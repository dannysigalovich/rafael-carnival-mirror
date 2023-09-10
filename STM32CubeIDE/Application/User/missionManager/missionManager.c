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
    }

    sys_mutex_new(&(manager->mutex));
}

void setMissions(MissionManager* manager, Mission* missions) {
    sys_mutex_lock(&(manager->mutex));

	for (int i = 0; i < MAX_MISSIONS; ++i) {
		manager->missions[i].mission_number = missions[i].mission_number;
		manager->missions[i].priority = missions[i].priority;
        manager->missions[i].assigned = false;
        manager->missions[i].completed = false;
        manager->missions[i].assigned_to = 0;
        // the mutex is init once in initializeMissionManager
	}
	manager->missionsSets = true;

    sys_mutex_unlock(&(manager->mutex));
}

int findAssignedMission(MissionManager* manager, uint8_t peopleNum){
	for (int i = 0; i < MAX_MISSIONS; ++i) {
		if (manager->missions[i].assigned && manager->missions[i].assigned_to == peopleNum){
			return manager->missions[i].mission_number;
		}
	}
	return 0;
}

// people here is meant for spike
unsigned short missionAssigned(MissionManager* manager, uint8_t peopleNum) {
	if (!manager->missionsSets) return 0; // in case we didnt get missions yet

	int selectedMission = findAssignedMission(manager, peopleNum);
	// if the people already assigned with a mission and ask again return the same mission
    if (selectedMission) return selectedMission;

    sys_mutex_lock(&(manager->mutex));

    int highestPriority = 0;

    for (int i = 0; i < MAX_MISSIONS; ++i) {
        if (!manager->missions[i].assigned && manager->missions[i].priority > highestPriority) {
            selectedMission = i;
            highestPriority = manager->missions[i].priority;
        }
    }

    if (selectedMission != -1) {
        manager->missions[selectedMission].assigned = true;
        manager->missions[selectedMission].assigned_to = peopleNum;
    }

    sys_mutex_unlock(&(manager->mutex));
    return selectedMission;
}

void completeInSuccess(MissionManager* manager, int missionNum) {
    sys_mutex_lock(&(manager->mutex));
    manager->missions[missionNum].completed = true;
    sys_mutex_unlock(&(manager->mutex));
}

void completeInFailure(MissionManager* manager, int missionNum) {
    sys_mutex_lock(&(manager->mutex));
    manager->missions[missionNum].completed = false;
    manager->missions[missionNum].assigned = false;
    manager->missions[missionNum].assigned_to = 0;
    sys_mutex_unlock(&(manager->mutex));
}
