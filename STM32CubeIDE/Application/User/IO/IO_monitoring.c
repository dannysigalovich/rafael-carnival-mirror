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
#include "I2C/i2c_config.h"

extern MissionManager misManager;
extern SpikeTaskData spikeData[MAX_SPIKES];

uint8_t launch(uint8_t spike){

//	LaunchError err = 0;
// TODO after defining error handling in this seq implement it
	elev_up(spike);

	uint32_t start = HAL_GetTick();
	while (HAL_GetTick() - start < ELEV_TIMEOUT){
		if (!is_spike_mid(spike) && is_spike_up(spike)){
			break;
		}
	}

	spikeData[spike].elevIsUp = true;
	sys_msleep(ELEV_ACTION_WAIT);
	spikeData[spike].elevIsUp = false;
	elev_down(spike);

	start = HAL_GetTick();
	while (HAL_GetTick() - start < ELEV_TIMEOUT){
		if (!is_spike_up(spike)){
			break;
		}
	}

	if (is_spike_up(spike)){
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

void check_disc_with_buttons(uint8_t spike){
	while (1){
			up = is_spike_up(spike);
			down = is_spike_down(spike);
			mid = is_spike_mid(spike);
	}
}

void check_up_down_with_leds(uint8_t spike){
	while (1){
		elev_up(spike);
		elev_down(spike);
	}
}

void startLaunchSequence(){
/*############## TESTS ##############*/
//	check_disc_with_buttons(2);
//	check_up_down_with_leds(2);
/*###################################*/

	sys_thread_new("luanchSeq", launchSequence, NULL, DEFAULT_THREAD_STACKSIZE, LAUNCH_SEQ_THREAD_PRIO);
}
