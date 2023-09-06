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


bool elevIsUp[MAX_MAOZ] = {0};
extern MissionManager misManager;

uint8_t launch(uint8_t maoz){

//	LaunchError err = 0;
// TODO after defining error handling in this seq implement it
	elev_up(maoz);

	uint32_t start = HAL_GetTick();
	while (HAL_GetTick() - start < ELEV_TIMEOUT){
		if (!is_maoz_mid(maoz) && is_maoz_up(maoz)){
			break;
		}
	}

	elevIsUp[maoz] = true;
	sys_msleep(ELEV_ACTION_WAIT);
	elevIsUp[maoz] = false;
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
				err = launch(misManager.missions[i].assigned_to);
				if (!err){
					misManager.missions[i].completed = true;
				}
				// TODO: handle launch errors
			}
		}
		sys_msleep(2);
	}
}

uint8_t up, mid, down;

void check_disc_with_buttons(uint8_t maoz){
	while (1){
			up = is_maoz_up(maoz);
			down = is_maoz_down(maoz);
			mid = is_maoz_mid(maoz);
	}
}

void check_up_down_with_leds(uint8_t maoz){
	while (1){
		elev_up(maoz);
		elev_down(maoz);
	}
}

void startLaunchSequence(){
/*############## TESTS ##############*/
//	check_disc_with_buttons(2);
//	check_up_down_with_leds(2);
/*###################################*/

	sys_thread_new("luanchSeq", launchSequence, NULL, DEFAULT_THREAD_STACKSIZE, LAUNCH_SEQ_THREAD_PRIO);
}
