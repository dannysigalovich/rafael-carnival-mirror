/*
 * ICD.h
 *
 *  Created on: Aug 15, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_ICD_ICD_H_
#define APPLICATION_USER_ICD_ICD_H_


#define MAX_WORDS 2
#define MAX_SECRET_SIZE 16
#define MISSIONS_HEADER_SIZE 5
#define SECURE_LAUNCH 135

#define PACKED __attribute__((packed, aligned(1)))

void ICD_handle(void *args);

typedef enum FlowState{
	Recv = 0,
	Process,
	Transmit,
} FlowState;

enum MessageTypeEnum{
	RequestEnum = 0,
	NavInsEnum = 1,
	MissionHeaderEnum = 2,
	WPDataEnum = 3,
	LaunchCmdEnum = 4,
	TargetLocationEnum = 5,
	AttackPolygonEnum = 6,
	GraveyardEnum = 7,
	FirFlyStatus= 8,
	InsStdEnum = 9,
	SecretCmdEnum = 10
};

typedef struct PACKED RequestMessage{
    unsigned char msgType;
    unsigned char requestedType;
    unsigned char msgSize;
} RequestMessage;



typedef struct PACKED NavFrameINS{
    unsigned char msgType; // according to MessageTypeEnum
    unsigned char msgId;   // Incremental
    unsigned short weeknumber;
    unsigned int mSec;
    double PositionLatitudeGeoRad;
    double PositionLongitudeGeoRad;
    float PositionAltitudeMeterAEL; // should be ASL ?
    float VelocityNorthMeterPerSec;
    float VelocityEastMeterPerSec;
    float VelocityDownMeterPerSec;
    float AzimuthDeg;      // 0 means North,  180 and -180 means South
    float PitchDeg;        // Right-handed rotation from local level around xaxis in degrees
    float RollDeg;         // Right-handed rotation from local level around yaxis in degrees
    unsigned int Status;
    unsigned char cs; // 0 means not valid else is valid

} NavFrameINS;

typedef struct PACKED NavFrameINSSTD {
	unsigned char msgType;
	unsigned char msgId;
	float StdLatitudeMeter;
	float StdLongitudeMeter;
	float StdAltitudeMeter;
	float StdVelocityNorth;
	float StdVelocityEast;
	float StdVelocityDown;
	float StdTrueHeadingDeg;
	float StdPitchDeg;
	float StdRollDeg;
	unsigned char cs;
} NavFrameINSSTD;


typedef struct PACKED LaunchCmd{
    unsigned char msgType; // according to MessageTypeEnum
    unsigned char msgId;   // Incremental
    unsigned short missionId;
    unsigned char secureLaunch;
    unsigned char cs;
} LaunchCmd;


typedef struct PACKED FireFlyStatus{
	unsigned char msgType;
    unsigned char msgId;
    unsigned char isReadyToLaunch; // 0 means NOT ready
    unsigned char batteryPercentage;
    unsigned int BITStatus; // bitmapped DWord according to the table
    unsigned char cs;
} FireFlyStatus;


typedef struct PACKED SecretCmd {
	unsigned char msgType; // according to MessageTypeEnum
	unsigned char secret1[MAX_SECRET_SIZE];
	unsigned char secret2[MAX_SECRET_SIZE];
	unsigned char cs;
} SecretCmd;

#endif /* APPLICATION_USER_ICD_ICD_H_ */
