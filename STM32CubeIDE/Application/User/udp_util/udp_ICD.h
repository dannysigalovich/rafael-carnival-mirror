/*
 * udp_ICD.h
 *
 *  Created on: Oct 24, 2023
 *      Author: eladsez
 */

#ifndef UDP_ICD_H_
#define UDP_ICD_H_

#include "ICD/ICD.h"
#include "main.h"

#define SYNC_SIZE 5
#define SYNC_STR "\xAA\xBB\xCC\xDD\xEE"
#define MAX_UDP_PACKET_SIZE 1024
#define MAX_UDP_DATA_SIZE (MAX_UDP_PACKET_SIZE - SYNC_SIZE - 1) // the -1 is for the msg type
#define MAX_BNET 2
#define MAX_LAUNCH_COMP 2 // max launch computer (at a time 2)


#define CONSTRUCT_SYNC(sync) \
    do { \
        memcpy(sync, SYNC_STR, SYNC_SIZE); \
    } while (0)
    
#define SYNCHECK(sync) \
    (memcmp(sync, "\xAA\xBB\xCC\xDD\xEE", 5) == 0)


typedef enum UdpMsgType{
    MissionsSet = 0,
    LogListReq = 1,
    LogFileReq = 2,
    LiveLogStartReq = 3,
    LiveLogStopReq = 4,
    BeehiveSetUp = 5,
    LiveLogResp = 6,
    LogListResp = 7, // not Relevant
    LogFileResp = 8, // not Relevant
	LaunchReq = 9,
	BeehiveLoadReq = 10,
} MsgType;


typedef struct PACKED UdpPacket{
    uint8_t sync[SYNC_SIZE];
    uint8_t msgType; // according to UdpMsgType enum
    uint8_t data[MAX_UDP_DATA_SIZE];// in request - empty, in response - the data
} UdpPacket;

typedef struct PACKED LaunchReqData{
    int spikeNum; // which spike to launch (-1 indicate Auto mode - launch all)
} LaunchReqData; // it's dumm to make it an int and not int8_t but its for easy development

typedef struct PACKED BeehiveLoadData{
	int8_t spikeToLoad[MAX_SPIKES]; // req to load spike into slots (1 is elevator up, 0 - none, -1 - elevator down)
} BeehiveLoadData;

typedef struct PACKED BeehiveSetUpData{
    uint8_t turnOnSpikes[MAX_SPIKES]; // those are the spikes that are (1 - exist, 0 - not) in the beehive and should turn on
    uint8_t turnOnBnet[MAX_BNET]; // those are the BNET we should enable
} BeehiveSetUpData;

typedef struct PACKED MissionsData{
    unsigned short spikePerMissions[MAX_SPIKES * MAX_LAUNCH_COMP]; // spike number 0 got mission spikePerMissions[0] (if -1 set we in Auto mode)
    unsigned short missions[MAX_MISSIONS];
} MissionsData;

typedef struct PACKED LiveLogData{ // send over udp when asked for
    uint8_t isSpikeInitialized[MAX_SPIKES];
    uint8_t isBnetInitialized[MAX_BNET];
	unsigned int BITStatus[MAX_SPIKES];
	unsigned char batteryPercentage[MAX_SPIKES];
	unsigned char isReadyToLaunch[MAX_SPIKES];
    char isLaunchedSuccesfully[MAX_SPIKES]; // 0 - not launched yet, 1 - launched successfully, -1 - launch failed
	char elevGoUp[MAX_SPIKES];
	char elevIsUp[MAX_SPIKES];
} LiveLog;


#endif /* UDP_ICD_H_ */
