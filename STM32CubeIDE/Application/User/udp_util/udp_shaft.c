/*
 * udp_shaft.c
 *
 *  Created on: Aug 27, 2023
 *      Author: eladsez
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "Novatel/navMesseging.h"
#include "INSBuffer/cyBuff.h"
#include "udp_conf.h"
#include "lwjson/json_parser/json_parser.h"
#include "ICD/ICD.h"
#include "missionManager/missionManager.h"
#include "logger/logger.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define SYNC_SIZE 5
#define check_sync(sync) (sync[0] == 0xAA && sync[1] == 0xBB && sync[2] == 0xCC && sync[3] == 0xDD && sync[4] == 0xEE)

/* Private variables ---------------------------------------------------------*/
CircularBuffer INSPVAXBuff;

MissionManager misManager;
char secret_words[2][MAX_SECRET_SIZE];

struct send_flag{
  ip_addr_t addr;
  u16_t port;
  bool send_log;
  uint32_t req_log_count; /* The requested log file */
  bool send_log_list;
} send_flag = {0};

/* Private function prototypes -----------------------------------------------*/
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void udp_shaft_thread(void* arg);
void handle_send(struct udp_pcb *send_pcb);

void build_dummy_missions(){ // TODO:  remove when needed
	Mission missions[MAX_MISSIONS];
	for (int i = 0; i < MAX_MISSIONS; ++i){
		missions[i].mission_number = i + 1;
		missions[i].priority = i + 1;
	}
	setMissions(&misManager, missions);
}


/**
  * @brief  Initialize the udp connection and all the data structures it uses (Also start its thread)
  * @param  none
  * @retval None
  */
void init_udp_broker(){
	initCircularBuffer(&INSPVAXBuff);

	initializeMissionManager(&misManager);

	memset(secret_words[0], 0, MAX_SECRET_SIZE);
	memset(secret_words[1], 0, MAX_SECRET_SIZE);

	build_dummy_missions();

	sys_thread_new("udp_connection", udp_shaft_thread, NULL, UDP_THREAD_STACKSIZE, UDP_THREAD_PRIO);
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_shaft_thread(void* arg)
{
  struct udp_pcb *upcb, *send_pcb;
  err_t err;

  /* Create a new UDP control block  */
  upcb = udp_new();

  if (upcb){
    /* Bind the upcb to the UDP_PORT port */
    /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
    err = udp_bind(upcb, IP_ADDR_ANY, UDP_PORT);

    if(err == ERR_OK){
      /* Set a receive callback for the upcb */
      udp_recv(upcb, udp_receive_callback, NULL);
    }
    else{
      udp_remove(upcb);
    }
  }
#ifdef LOGGER_ON
  send_pcb = udp_new();
  if (send_pcb){
    while(1){
      handle_send(send_pcb);
    }
  }
#endif

  while(1){ /* in case the send pcb failed to create */
    sys_msleep(5); /* for the cpu to not get stuck */
  }
}

void handle_send(struct udp_pcb *send_pcb){
  
  if (send_flag.send_log_list || send_flag.send_log){
    /* allocate pbuf from RAM*/

	  char buff[MAX_LOG_FILE_SIZE] = {0};

	  int size = send_flag.send_log ? read_log(buff, MAX_LOG_FILE_SIZE, send_flag.req_log_count) : list_log_files(buff, MAX_LOG_FILE_SIZE);

	  if (size < 0) return;

	  struct pbuf *p_tx = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);

	  if(p_tx == NULL) return;

	  /* Copy the log file to the p_tx buffer */
	  pbuf_take(p_tx, buff, size);

	  udp_sendto(send_pcb, p_tx, &send_flag.addr, send_flag.port);
	  /* Free the p_tx buffer */
	  pbuf_free(p_tx);

	  if (send_flag.send_log){ // reset the flag we just take care of
		send_flag.send_log = false;
	  }
	  else{
		send_flag.send_log_list = false;
	  }

  }
}

uint8_t INS_sync_check(struct pbuf *pbuf){

  INS_header *header = pbuf->payload;
	uint8_t *sync = header->sync;

	uint8_t ret = pbuf->len == sizeof(INSPVAX); // check that we got a hole packet

	return ret && sync[0] == 0xAA && sync[1] == 0x44 && sync[2] == 0x12 && header->msgID == INSPVAXType;
}

uint8_t missions_sync_check(struct pbuf *pbuf){
  if (pbuf->len < SYNC_SIZE){
    return 0;
  }
	uint8_t *sync = pbuf->payload;
	return check_sync(sync);
}

uint8_t log_req_sync_check(struct pbuf *pbuf){
  if (pbuf->len < SYNC_SIZE + sizeof(REQ_LOG)){
    return 0;
  }
  uint8_t *sync = pbuf->payload;
  bool bla = strncmp((char*)sync + SYNC_SIZE, REQ_LOG, strlen(REQ_LOG)) == 0;
  return check_sync(sync) && bla;
}

uint8_t log_list_req_sync_check(struct pbuf *pbuf){
  if (pbuf->len < SYNC_SIZE + sizeof(REQ_LOG_LIST)){
    return 0;
  }
  uint8_t *sync = pbuf->payload;
  return check_sync(sync) && strncmp((char*)sync + SYNC_SIZE, REQ_LOG_LIST, strlen(REQ_LOG_LIST)) == 0;
}


void parse_packet(struct pbuf *pbuf, const ip_addr_t *addr, u16_t port){

    if(INS_sync_check(pbuf)){
		  writeToBuff(&INSPVAXBuff,pbuf->payload , sizeof(INSPVAX));
    }
    else if (log_req_sync_check(pbuf)){
      logger_sync();
      send_flag.send_log = true;
      send_flag.req_log_count = *(uint32_t *)(pbuf->payload + SYNC_SIZE + strlen(REQ_LOG)); // + 1 for the \0
      send_flag.port = port;
      memcpy(&send_flag.addr, addr, sizeof(ip_addr_t));
    }
    else if (log_list_req_sync_check(pbuf)){
      logger_sync();
      send_flag.send_log_list = true;
      send_flag.port = port;
      memcpy(&send_flag.addr, addr, sizeof(ip_addr_t));
    }
    else if (missions_sync_check(pbuf)){ /* the missions must be at the end */
    	Mission missions[MAX_MISSIONS] = {0};
    	parse_missions(pbuf->payload + MISSIONS_HEADER_SIZE, missions, secret_words);
    	setMissions(&misManager, missions);
    }
    else{
      // error
    }
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  struct pbuf *p_tx;

  /* allocate pbuf from RAM*/
  p_tx = pbuf_alloc(PBUF_TRANSPORT,p->len, PBUF_RAM);

  if(p_tx != NULL)
  {
    pbuf_take(p_tx, (char*)p->payload, p->len);

    parse_packet(p_tx, addr, port);

    /* Free the p_tx buffer */
    pbuf_free(p_tx);

    /* Free the p buffer */
    pbuf_free(p);
  }
}
