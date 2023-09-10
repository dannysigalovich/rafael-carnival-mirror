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
        manager->missions[i].assigned_to = -1;
        // the mutex is init once in initializeMissionManager
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
	if (!manager->missionsSets) return 0; // in case we didnt get missions yet

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
    manager->missions[missionIndex].completed = false;
    manager->missions[missionIndex].assigned = false;
    manager->missions[missionIndex].assigned_to = -1;
    sys_mutex_unlock(&(manager->mutex));
}
