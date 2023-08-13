/*
 * tcp_server.c
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#include "Novatel/navMesseging.h"
#include "tcp_util/tcp_echo_server.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/apps/fs.h"
#include "cmsis_os.h"
#include "CyclicBuffer/cyBuff.h"
#include "stdio.h"

extern CircularBuffer CPTcircularBuff;



void CPT_broker_thread(void *arg){

	char buff[1024];

	while (1){
		if (!isCircularBufferEmpty(&CPTcircularBuff)){
			uint32_t size = CPTcircularBuff.count;
			readFromBuff(&CPTcircularBuff, buff, size);
			for (int i = 0; i < size; ++i){
				printf("%c", buff[i]);
			}
			printf("\r\n");
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
  sys_thread_new("CPT7_tcp_server", tcp_echoserver_init, NULL, DEFAULT_THREAD_STACKSIZE, TCPSERVER_THREAD_PRIO);
  sys_thread_new("CPT7_broker", CPT_broker_thread, NULL, DEFAULT_THREAD_STACKSIZE, CPT7_BROKER_THREAD_PRIO);
}
