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
#include "I2C/i2c_config.h"
#include "Novatel/navMesseging.h"
#include "INSBuffer/cyBuff.h"
#include "udp_conf.h"
#include "lwjson/json_parser/json_parser.h"
#include "ICD/ICD.h"
#include "missionManager/missionManager.h"
#include "logger/logger.h"
#include "udp_ICD.h"
#include "IO/IO.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define CPY_ADDR(ip, port) \
    do { \
        memcpy(&send_flag.addr, (ip), sizeof(ip_addr_t)); \
        send_flag.port = (port); \
    } while (0)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern SpikeTaskData spikeData[MAX_SPIKES];
extern uint8_t isLaunchStarted;
char secret_words[2][MAX_SECRET_SIZE];
CircularBuffer INSPVAXBuff;
MissionManager misManager;

struct send_flag{
	ip_addr_t addr;
	u16_t port;
	bool send_log;
	uint32_t req_log_count; /* The requested log file */
	bool send_log_list;
	bool send_live;
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
  struct udp_pcb *upcb;
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

  struct udp_pcb *send_pcb = udp_new();

  if (send_pcb){
    while(1){
      handle_send(send_pcb);
      sys_msleep(5); /* for the cpu to not get stuck */
    }
  }

  while(1){ /* in case the send pcb failed to create */
    sys_msleep(5); /* for the cpu to not get stuck */
  }
}

int build_live_log(uint8_t *buff, uint32_t size){

	if (size < sizeof(LiveLog))
		return -1;

	LiveLog live = {0};
	for (int i = 0; i < MAX_SPIKES; ++i){
		live.IsInitialized[i] = spikeData[i].initState;
		live.batteryPercentage[i] = spikeData[i].currStatus.batteryPercentage;
		live.BITStatus[i] = spikeData[i].currStatus.BITStatus;
		live.isReadyToLaunch[i] = spikeData[i].currStatus.isReadyToLaunch;
		live.elevGoUp[i] = spikeData[i].elevGoUp;
		live.elevIsUp[i] = spikeData[i].elevGoUp;
	}

	memcpy(buff, &live, sizeof(LiveLog));

	return sizeof(LiveLog);
}

void handle_send(struct udp_pcb *send_pcb){
  
  int size = 0;
  UdpPacket packet;
  CONSTRUCT_SYNC(packet.sync);

  if (send_flag.send_live){
    packet.msgType = LiveLogResp;
    size = build_live_log(packet.data, MAX_UDP_DATA_SIZE);
  }
  else if (send_flag.send_log){
    packet.msgType = LogFileResp;
    size = read_log((char *)packet.data, send_flag.req_log_count, MAX_UDP_DATA_SIZE);
    send_flag.send_log = false;
  }
  else if (send_flag.send_log_list){
    packet.msgType = LogListResp;
    size = list_log_files((char *)packet.data, MAX_UDP_DATA_SIZE);
    send_flag.send_log_list = false;
  }
  else{
    return;
  }


  if (size < 0) return;

  /* allocate pbuf from RAM*/
  struct pbuf *p_tx = pbuf_alloc(PBUF_TRANSPORT, sizeof(UdpPacket), PBUF_RAM);
  if(p_tx == NULL) return;

  /* Copy the log file to the p_tx buffer */
  pbuf_take(p_tx, &packet, sizeof(UdpPacket));

  udp_sendto(send_pcb, p_tx, &send_flag.addr, send_flag.port);
  /* Free the p_tx buffer */
  pbuf_free(p_tx);

}

uint8_t INS_sync_check(struct pbuf *pbuf){
	INS_header *header = pbuf->payload;
	uint8_t *sync = header->sync;
	uint8_t ret = pbuf->len == sizeof(INSPVAX); // check that we got a hole packet
	return ret && sync[0] == 0xAA && sync[1] == 0x44 && sync[2] == 0x12 && header->msgID == INSPVAXType;
}

void beehive_setUp(BeehiveSetUpData *data){

  for (int i = 0; i < MAX_SPIKES; ++i){
    if (data->existing_spikes[i] == Init && spikeData[i].initState == NoInit){
      spikeData[i].initState = Init;
      I2C_start_listen(i);
      turn_on_spike(i);
      spikeData[i].initState = spikeData[i].initState == SpikeRelayStarted ?
                               Done : spikeData[i].initState;
    }
  }
  for (int i = 0; i < MAX_BNET; ++i){
    if (data->existing_BNET[i] == Init){
      turn_on_BNET(i);
    }
  }
}

void parse_packet(struct pbuf *pbuf, const ip_addr_t *addr, u16_t port){

    if(INS_sync_check(pbuf)){
		  writeToBuff(&INSPVAXBuff,pbuf->payload , sizeof(INSPVAX));
    }
    else if (!SYNCHECK(pbuf->payload) || pbuf->len < SYNC_SIZE + 1) return; // check if the packet is valid

    UdpPacket *packet = pbuf->payload;

    switch (packet->msgType){
    	case MissionsSecretJson:
        Mission missions[MAX_MISSIONS] = {0};
    	  parse_missions((const char* )(packet->data), missions, secret_words);
    	  setMissions(&misManager, missions);
    		break;
    	case LogListReq:
        send_flag.send_log_list = true;
        CPY_ADDR(addr, port);
    		break;
    	case LogFileReq:
        send_flag.send_log = true;
        send_flag.req_log_count = ((uint32_t *)(packet->data))[0];
        CPY_ADDR(addr, port);
    		break;
    	case LiveLogStartReq:
        send_flag.send_live = true;
        CPY_ADDR(addr, port);
    		break;
    	case LiveLogStopReq:
        send_flag.send_live = false;
    		break;
      case BeehiveSetUp:
    	  beehive_setUp((BeehiveSetUpData *)packet->data);
        break;
      case LaunchReq:
    	  isLaunchStarted = true;
    	  break;
      default:
        break;
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
