/*
 * tcp_server.c
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#include <INSBuffer/cyBuff.h>
#include "Novatel/navMesseging.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/apps/fs.h"
#include "cmsis_os.h"
#include <stdio.h>


// ###################################################################### UNSUPPORTED ######################################################################

/**
  * @brief  Initialize the tcp server (start its thread)
  * @param  none
  * @retval None
  */
void init_novatel_debug()
{
//	sys_thread_new("CPT7_debug_thread", CPT_debug_thread, NULL, DEFAULT_THREAD_STACKSIZE, CPT7_BROKER_THREAD_PRIO);
}



