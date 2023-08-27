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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CircularBuffer INSPVABuff;
CircularBuffer INSSTDBuff;

/* Private function prototypes -----------------------------------------------*/
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
void udp_shaft_thread(void* arg);

/**
  * @brief  Initialize the udp connection and all the data structures it uses (Also start its thread)
  * @param  none
  * @retval None
  */
void init_udp_broker(){

	initCircularBuffer(&INSPVABuff, INSPVAType);
	initCircularBuffer(&INSSTDBuff, INSSTDType);

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
 	  sys_msleep(1000); /* for the cpu to not poll */
   }
}

uint8_t sync_check(struct pbuf *pbuf){

    INS_header *header = pbuf->payload;
	uint8_t *sync = header->sync;

	uint8_t ret = sync[0] == 0xAA && sync[1] == 0x44 && sync[2] == 0x12; // check the sync symbols of the header

	ret = ret && (pbuf->len == sizeof(INSPVA) || pbuf->len == sizeof(INSSTDEV)); // check that we got a hole packet

	return ret;
}

void parse_packet(struct pbuf *pbuf){

	// TODO: add logic to check if it is a missions_secret packet
    INS_header *header = pbuf->payload;

    if (!sync_check(pbuf)){// check if the pkt is meant for us as part of INS log
    	return;
    }

    switch (header->msgID){
    case INSPVAType:
    	writeToBuff(&INSPVABuff, (INSPVA *) pbuf->payload, sizeof(INSPVA));
    	break;
    case INSSTDType:
    	writeToBuff(&INSSTDBuff, (INSSTDEV *) pbuf->payload, sizeof(INSSTDEV));
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
