/*
 * missionManager.c
 *
 *  Created on: Aug 31, 2023
 *      Author: eladsez
 */
#include "missionManager.h"

void initializeMissionManager(MissionManager* manager) {
    for (int i = 0; i < MAX_MISSIONS; ++i) {
        memset(manager->missions, 0, sizeof(manager) * MAX_MISSIONS);
        manager->missionsSets = false;
        manager->isSpikeFail[i] = false;
    }

    sys_mutex_new(&(manager->mutex));
}

uint8_t find_assignee_index(unsigned short *spikePerMissions, unsigned short missionToFind){
    for (uint8_t i = 0; i < MAX_SPIKES * MAX_LAUNCH_COMP; ++i) {
        if (spikePerMissions[i] == missionToFind) return i;
    }
    return -1;
}

void setMissions(MissionManager* manager, MissionsData* missionsData) {
    sys_mutex_lock(&(manager->mutex));

    unsigned short spikePerMissions[MAX_SPIKES * MAX_LAUNCH_COMP];
    memcpy(spikePerMissions, missionsData->spikePerMissions, sizeof(spikePerMissions)); // this is must be that way because of unaligned memory of struct missionsData

	for (int i = 0; i < MAX_MISSIONS; ++i) {
        manager->missions[i].assigned_to = find_assignee_index(spikePerMissions, missionsData->missions[i]);
		manager->missions[i].mission_number = missionsData->missions[i];
		manager->missions[i].priority = 0;
        manager->missions[i].assigned = manager->missions[i].assigned_to != -1 ? true : false;
        manager->missions[i].completed = false;
	}
	manager->missionsSets = true;

    sys_mutex_unlock(&(manager->mutex));
}

/* returns the index of the mission assigned to the given peopleNum, or -1 if none */
int findAssignedMission(MissionManager* manager, uint8_t peopleNum){
	for (int i = 0; i < MAX_MISSIONS; ++i) {
		if (manager->missions[i].assigned && manager->missions[i].assigned_to == peopleNum){
			return i;
		}
	}
	return -1;
}

/* returns the mission number assigned to the given peopleNum, or 0 if none 
   it will assign a new mission if there is a higher priority mission than the one assigned to the spike
*/
unsigned short missionAssigned(MissionManager* manager, uint8_t peopleNum) {
	if (!manager->missionsSets || manager->isSpikeFail[peopleNum]) return 0; // in case we didnt get missions yet

    int highestMissionIndex = -1, highestPriority = -1, oldMissionIndex = -1;

	oldMissionIndex = findAssignedMission(manager, peopleNum);

    sys_mutex_lock(&(manager->mutex));

    // find the highest priority mission that is not assigned to anyone
    for (int i = 0; i < MAX_MISSIONS; ++i) {
        if (!manager->missions[i].assigned && manager->missions[i].priority > highestPriority) {
            highestMissionIndex = i;
            highestPriority = manager->missions[i].priority;
        }
    }

    // if there is a higher priority mission than the one assigned to the spike, unassign the old one and assign the new one
    if (highestMissionIndex != -1 && oldMissionIndex != -1
        && manager->missions[highestMissionIndex].priority > manager->missions[oldMissionIndex].priority) {
        manager->missions[oldMissionIndex].assigned = false;
        manager->missions[oldMissionIndex].assigned_to = -1;
        manager->missions[highestMissionIndex].assigned = true;
        manager->missions[highestMissionIndex].assigned_to = peopleNum;
    }
    // if there is no old mission assigned to the spike, assign the new one
    else if (highestMissionIndex != -1 && oldMissionIndex == -1) {
        manager->missions[highestMissionIndex].assigned = true;
        manager->missions[highestMissionIndex].assigned_to = peopleNum;
    }
    else {// if there is no new mission to assign, return the old mission
        highestMissionIndex = oldMissionIndex;
    }

    sys_mutex_unlock(&(manager->mutex));
    // the return line is like this in case there is no old mission assigned to the spike and no new mission to assign
    return highestMissionIndex == -1 ? 0 : manager->missions[highestMissionIndex].mission_number;
}

void completeInSuccess(MissionManager* manager, int missionIndex) {
    sys_mutex_lock(&(manager->mutex));
    manager->missions[missionIndex].completed = true;
    sys_mutex_unlock(&(manager->mutex));
}

void completeInFailure(MissionManager* manager, int missionIndex) {
    sys_mutex_lock(&(manager->mutex));
    manager->isSpikeFail[manager->missions[missionIndex].assigned_to] = true;
    manager->missions[missionIndex].completed = false;
    manager->missions[missionIndex].assigned = false;
    manager->missions[missionIndex].assigned_to = -1;
    sys_mutex_unlock(&(manager->mutex));
}
