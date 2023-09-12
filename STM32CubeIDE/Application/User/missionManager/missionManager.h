/*
 * missionManager.h
 *
 *  Created on: Aug 31, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_MISSIONMANAGER_MISSIONMANAGER_H_
#define APPLICATION_USER_MISSIONMANAGER_MISSIONMANAGER_H_

#include "main.h"
#include "lwip/sys.h" // Include the necessary header for STM32 semaphore functions
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_MISSIONS 4

typedef struct {
	unsigned short mission_number;
    int priority;
    bool assigned;
    bool completed;
    uint8_t assigned_to;
    sys_mutex_t mutex;
} Mission;

typedef struct {
    Mission missions[MAX_MISSIONS];
    bool missionsSets;
    // true if the spike is fail in luanch sequence, false otherwise. for us to know to not send him missions anymore
    bool isSpikeFail[MAX_SPIKES];
    sys_mutex_t mutex;
} MissionManager;


void initializeMissionManager(MissionManager* manager);
void setMissions(MissionManager* manager, Mission* missions);
unsigned short missionAssigned(MissionManager* manager, uint8_t peopleNum);
void completeInSuccess(MissionManager* manager, int missionNum);
void completeInFailure(MissionManager* manager, int missionNum);

#endif /* APPLICATION_USER_MISSIONMANAGER_MISSIONMANAGER_H_ */
