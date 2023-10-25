/*
 * udp_ICD.h
 *
 *  Created on: Oct 24, 2023
 *      Author: eladsez
 */

#ifndef UDP_ICD_H_
#define UDP_ICD_H_

#include "ICD/ICD.h"

#define SYNC_SIZE 5
#define SYNC_STR "\xAA\xBB\xCC\xDD\xEE"
#define MAX_UDP_PACKET_SIZE 1024
#define MAX_UDP_DATA_SIZE (MAX_UDP_PACKET_SIZE - SYNC_SIZE - 1) // the -1 is for the msg type

#define CONSTRUCT_SYNC(sync) \
    do { \
        memcpy(sync, SYNC_STR, SYNC_SIZE); \
    } while (0)
    
#define SYNCHECK(sync) \
    (memcmp(sync, "\xAA\xBB\xCC\xDD\xEE", 5) == 0)


typedef enum UdpMsgType{
    MissionsSecretJson = 0,
    LogListReq = 1,
    LogFileReq = 2,
    LiveLogStartReq = 3,
    LiveLogStopReq = 4,
    BeehiveSetUp = 5,
    LiveLogResp = 6,
    LogListResp = 7,
    LogFileResp = 8,
} MsgType;


typedef struct PACKED UdpPacket{
    uint8_t sync[SYNC_SIZE];
    uint8_t msgType; // according to UdpMsgType enum
    uint8_t data[MAX_UDP_DATA_SIZE];// in request - empty, in response - the data
} UdpPacket;


typedef struct PACKED BeehiveSetUpData{
    uint8_t existing_spikes[MAX_SPIKES]; // those are the spikes that are (1 - exist, 0 - not) in the beehive and should turn on
} BeehiveSetUpData;


typedef struct PACKED LiveLogData{ // send over udp when asked for
    unsigned int IsInitialized[MAX_SPIKES];
	unsigned int BITStatus[MAX_SPIKES];
	unsigned char batteryPercentage[MAX_SPIKES];
	unsigned char isReadyToLaunch[MAX_SPIKES];
    char isLaunchedSuccesfully[MAX_SPIKES]; // 0 - not launched yet, 1 - launched successfully, -1 - launch failed
	char elevGoUp[MAX_SPIKES];
	char elevIsUp[MAX_SPIKES];
} LiveLog;


#endif /* UDP_ICD_H_ */