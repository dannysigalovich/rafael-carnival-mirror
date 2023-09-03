/*
 * IO_monitoring.c
 *
 *  Created on: Aug 30, 2023
 *      Author: eladsez
 */
#include <IO/IO.h>
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/apps/fs.h"
#include "missionManager/missionManager.h"


uint8_t isLaunchStarted = 0;
extern MissionManager misManager;

uint8_t luanch(uint8_t maoz){

//	LaunchError err = 0;
// TODO after defining error handling in this seq implement it
	elev_up(maoz);

	uint32_t start = HAL_GetTick();
	while (HAL_GetTick() - start < ELEV_TIMEOUT){
		if (!is_maoz_mid(maoz) && is_maoz_up(maoz)){
			break;
		}
	}

	sys_msleep(ELEV_ACTION_WAIT);
	elev_down(maoz);

	start = HAL_GetTick();
	while (HAL_GetTick() - start < ELEV_TIMEOUT){
		if (!is_maoz_up(maoz)){
			break;
		}
	}

	if (is_maoz_up(maoz)){
		// TODO: send status including Dx-3
	}

	return 0;
}

void launchSequence(void *args){
	LaunchError err = NoError;
	while(1){
		for (int i = 0; i < MAX_MISSIONS; ++i){
			err = 0;
			if (misManager.missions[i].completed) continue;
			if (misManager.missions[i].assigned){
				err = luanch(misManager.missions[i].assigned_to);
				if (!err){
					misManager.missions[i].completed = true;
				}
				// TODO: handle launch errors
			}
		}
		sys_msleep(2);
	}
}


void startLaunchSequence(){
	sys_thread_new("luanchSeq", launchSequence, NULL, DEFAULT_THREAD_STACKSIZE, LAUNCH_SEQ_THREAD_PRIO);
}

uint8_t isLaunchSwitchOn(){
	return isLaunchStarted;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if (GPIO_Pin == LAUNCH_GPIO_PIN){
	  isLaunchStarted = 1;
  }
}
