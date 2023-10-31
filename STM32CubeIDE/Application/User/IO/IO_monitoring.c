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

	elev_up(spike);

	uint32_t start = HAL_GetTick();
	while (HAL_GetTick() - start < ELEV_TIMEOUT){
		if (is_spike_up(spike)){
			status = SpikeNotFreeAndElevUp;
			break;
		}
	}

	if (status == ElevStaysDown){
		elev_down(spike);
		return ElevStaysDown;
	}

	spikeData[spike].elevGoUp = false;

	spikeData[spike].elevIsUp = true;
	sys_msleep(ELEV_ACTION_WAIT);
	spikeData[spike].elevIsUp = false;
	elev_down(spike);


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

uint8_t up, mid;

void check_disc_with_buttons(){
	for (int i = 0; i < 4; ++i){
		for (int j = 0 ; j < 2; ++j){
			up = is_spike_up(i);
			mid = is_spike_mid(i);
		}
	}
}

void power_up_spike(){
	for (int i = 0; i < 4; ++i){
		turn_on_spike(i);
	}
}
void check_up_down_with_leds(){
	for (int i = 0; i < 4; ++i){
		elev_up(i);
		elev_down(i);
	}
}

void check_bnets(){
	for (int i = 0; i < 2; ++i){
			turn_on_BNET(i);
			turn_off_BNET(i);
	}
}

void startLaunchSequence(){
/*############## TESTS ##############*/
//	 check_disc_with_buttons();
//	 check_up_down_with_leds(2);
//	power_up_sipike();
//	check_bnets();
/*###################################*/

	sys_thread_new("luanchSeq", launchSequence, NULL, DEFAULT_THREAD_STACKSIZE, LAUNCH_SEQ_THREAD_PRIO);
}
