/*
 * tcp_server.c
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#include <tcp_util/tcp_server.h>
#include "Novatel/navMesseging.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/apps/fs.h"
#include "cmsis_os.h"
#include "CyclicBuffer/cyBuff.h"
#include "stdio.h"

extern CircularBuffer CPTcircularBuff;


void printINSPVAFields(const INSPVA *inspva) {
    printf("Week: %lu\n", inspva->week);
    printf("Seconds: %f\n", inspva->seconds);
    printf("Latitude: %f\n", inspva->latitude);
    printf("Longitude: %f\n", inspva->longitude);
    printf("Height: %f\n", inspva->height);
    printf("North Velocity: %f\n", inspva->northVelocity);
    printf("East Velocity: %f\n", inspva->eastVelocity);
    printf("Up Velocity: %f\n", inspva->upVelocity);
    printf("Roll: %f\n", inspva->roll);
    printf("Pitch: %f\n", inspva->pitch);
    printf("Azimuth: %f\n", inspva->azimuth);
    printf("Status: %d\n", inspva->status);
    printf("CRC: %u\n", inspva->crc);
}



void CPT_debug_thread(void *arg){

	INSPVA ins;

	while (1){
		if (!isCircularBufferEmpty(&CPTcircularBuff)){
			readFromBuff(&CPTcircularBuff, &ins);
			printINSPVAFields(&ins);
		}
		else{
			sys_msleep(100);
		}
	}
}

/**
  * @brief  Initialize the tcp server (start its thread)
  * @param  none
  * @retval None
  */
void init_CPT7_broker()
{
	sys_thread_new("CPT7_tcp_server", tcp_server_init, NULL, DEFAULT_THREAD_STACKSIZE, TCPSERVER_THREAD_PRIO);
//	sys_thread_new("CPT7_debug_thread", CPT_debug_thread, NULL, DEFAULT_THREAD_STACKSIZE, CPT7_BROKER_THREAD_PRIO);
}
