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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

#define MIN_PAYLOAD_SIZE 20 // the minimum payload size if it smaller then this we ignore (we can't check if its ours)

/* Private variables ---------------------------------------------------------*/
CircularBuffer INSPVAXBuff;

MissionManager misManager;
char secret_words[2][MAX_SECRET_SIZE];

/* Private function prototypes -----------------------------------------------*/
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void udp_shaft_thread(void* arg);

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

	sys_thread_new("udp_connection", udp_shaft_thread, NULL, DEFAULT_THREAD_STACKSIZE, UDP_THREAD_PRIO);
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_shaft_thread(void* arg)
{
   struct udp_pcb *upcb;
   err_t err;

   /* Create a new UDP control block  */
   upcb = udp_new();

   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_PORT);

      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_receive_callback, NULL);
      }
      else
      {
        udp_remove(upcb);
      }
   }

   while(1){
	   sys_msleep(5); /* for the cpu to not poll */
   }
}


uint8_t INS_sync_check(struct pbuf *pbuf){

    INS_header *header = pbuf->payload;
	uint8_t *sync = header->sync;

	uint8_t ret = pbuf->len == sizeof(INSPVAX); // check that we got a hole packet

	return ret && sync[0] == 0xAA && sync[1] == 0x44 && sync[2] == 0x12 && header->msgID == INSPVAXType;
}

uint8_t missions_sync_check(struct pbuf *pbuf){
	uint8_t *sync = pbuf->payload;
	return sync[0] == 0xAA && sync[1] == 0xBB && sync[2] == 0xCC && sync[3] == 0xDD && sync[4] == 0xEE;
}


void parse_packet(struct pbuf *pbuf){
	if (pbuf->len < MIN_PAYLOAD_SIZE){
		return;
	}

    if (missions_sync_check(pbuf)){
    	Mission missions[MAX_MISSIONS] = {0};
    	parse_missions(pbuf->payload + MISSIONS_HEADER_SIZE, missions, secret_words);
    	setMissions(&misManager, missions);
    }
    else if(INS_sync_check(pbuf)){
		writeToBuff(&INSPVAXBuff,pbuf->payload , sizeof(INSPVAX));
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

    parse_packet(p_tx);

// ###################### comment this because we don't send anything (yet) ######################
//
//    /* Connect to the remote client */
//    udp_connect(upcb, addr, UDP_CLIENT_PORT);
//
//    /* Tell the client that we have accepted it */
//    udp_send(upcb, p_tx);
//
//    /* free the UDP connection, so we can accept new clients */
//    udp_disconnect(upcb);
//
// ###############################################################################################

    /* Free the p_tx buffer */
    pbuf_free(p_tx);

    /* Free the p buffer */
    pbuf_free(p);
  }
}
