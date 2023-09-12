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

LaunchError launch(uint8_t spike){

	LaunchError status = ElevStaysDown;

	elev_up(spike);

	uint32_t start = HAL_GetTick();
	while (HAL_GetTick() - start < ELEV_TIMEOUT){
		if (!is_spike_mid(spike) && is_spike_up(spike)){
			status = SpikeNotFreeAndElevUp;
			break;
		}
	}

	if (status == ElevStaysDown){
		elev_down(spike);
		return ElevStaysDown;
	}

	spikeData[spike].elevIsUp = true;
	sys_msleep(ELEV_ACTION_WAIT);
	spikeData[spike].elevIsUp = false;
	elev_down(spike);


	if (is_spike_up(spike)){
		return SpikeNotFreeAndElevUp;
	}
	if (is_spike_down(spike)){
		return SpikeNotFreeAndElevDown;
	}
	return NoError;
}

void launchSequence(void *args){
	LaunchError err = NoError;

	printf("Launch sequence monitoring started\r\n");

	while(1){
		for (int i = 0; i < MAX_MISSIONS; ++i){
			err = 0;
			if (misManager.missions[i].completed) continue;
			if (misManager.missions[i].assigned){
				printf("start launching mission %d with spike %d\r\n",
				misManager.missions[i].mission_number, misManager.missions[i].assigned_to + 1);
				err = launch(misManager.missions[i].assigned_to);
				if (err == NoError){
					completeInSuccess(&misManager, i);
					printf("Spike %d launched mission %d successfully\r\n",
					misManager.missions[i].assigned_to + 1, misManager.missions[i].mission_number);
				}
				else if (err == SpikeNotFreeAndElevUp){
					// when the elevator stays up is a fatal error, we need to stop the launch sequence and wait for manual intervention
					printf("##### FATAL ERROR ##### Spike %d is not free and elevator is stuck up! stop all.\r\n",
					misManager.missions[i].assigned_to + 1);
					break;
				}
				else if (err == ElevStaysDown || err == SpikeNotFreeAndElevDown){
					// when the elevator stays down is not a fatal error,
					// we need to complete the mission in failure for it to be assigned to another spike
					completeInFailure(&misManager, i);
					printf("Spike %d launched mission %d in failure, the mission will be assigned to another spike \r\n",
					misManager.missions[i].assigned_to + 1, misManager.missions[i].mission_number);
				}
			}
		}
		sys_msleep(1);
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
