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
#include "logger/logger.h"

extern MissionManager misManager;
extern SpikeTaskData spikeData[MAX_SPIKES];

LaunchError launch(uint8_t spike){

	LaunchError status = ElevStaysDown;

	spikeData[spike].elevGoUp = true;

	elev_up(spike);

	uint32_t start = HAL_GetTick();
	while (HAL_GetTick() - start < ELEV_TIMEOUT){
		if (is_spike_up(spike)){
			status = SpikeNotFreeAndElevUp;
			break;
		}
	}

	if (status == ElevStaysDown){
//		elev_down(spike);
		return ElevStaysDown;
	}

	spikeData[spike].elevGoUp = false;

	spikeData[spike].elevIsUp = true;
	sys_msleep(ELEV_ACTION_WAIT);
	spikeData[spike].elevIsUp = false;
//	elev_down(spike);


//	if (is_spike_up(spike)){
//		return SpikeNotFreeAndElevUp;
//	}
//	if (is_spike_down(spike)){
//		return SpikeNotFreeAndElevDown;
//	}
	return NoError;
}

void launchSequence(void *args){
	LaunchError err = NoError;

	while(1){
		for (int i = 0; i < MAX_MISSIONS; ++i){
			err = 0;
			if (misManager.missions[i].completed ||
					/* This line stop the launching in case the spike got a mission but not need to be launch yet */
			   (misManager.missions[i].assigned && !spikeData[misManager.missions[i].assigned_to].part_decision)) continue;
			if (misManager.missions[i].assigned){
				log_info("launching mission %d to spike %d", i, misManager.missions[i].assigned_to);
				err = launch(misManager.missions[i].assigned_to);
				if (err == NoError){
					log_info("launching mission %d to spike %d completed successfully", i, misManager.missions[i].assigned_to);
					completeInSuccess(&misManager, i);
				}
				else if (err == SpikeNotFreeAndElevUp){
					log_critical("launching mission %d to spike %d failed, the elevator is up and the spike is not free", i, misManager.missions[i].assigned_to);
					log_critical("Stoping the launch sequence and waiting for manual intervention");
					// when the elevator stays up is a fatal error, we need to stop the launch sequence and wait for manual intervention
					break;
				}
				else if (err == ElevStaysDown || err == SpikeNotFreeAndElevDown){
					// when the elevator stays down is not a fatal error,
					// we need to complete the mission in failure for it to be assigned to another spike
					log_warning("launching mission %d to spike %d failed, the mission will assign to another spike", i, misManager.missions[i].assigned_to);
					completeInFailure(&misManager, i);
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
//	 check_disc_with_buttons(1);
	// check_up_down_with_leds(2);
/*###################################*/

	sys_thread_new("luanchSeq", launchSequence, NULL, DEFAULT_THREAD_STACKSIZE, LAUNCH_SEQ_THREAD_PRIO);
}
