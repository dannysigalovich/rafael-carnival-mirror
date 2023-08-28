/*
 * ICD.h
 *
 *  Created on: Aug 15, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_ICD_ICD_H_
#define APPLICATION_USER_ICD_ICD_H_

#include "I2C/i2c_config.h"
#include "main.h"

#define MAX_MISSIONS 8
#define MAX_WORDS 2
#define MAX_SECRET_SIZE 16
#define MISSIONS_HEADER_SIZE 5

void ICD_handle(void *args);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);

enum FlowState{
	Recv = 0,
	Process,
	Transmit,
};

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

typedef struct RequestMessage{
    unsigned char msgType;
    unsigned char requestedType;
    unsigned char msgSize;
} RequestMessage;



typedef struct NavFrameINS{
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

typedef struct NavFrameINSSTD {
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
}NavFrameINSSTD ;

enum AltitudeTypeEnum {
	RelativeEnum = 0,
	PercentageEnum,
	ASLEnum,
	AGLEnum,
	AboveTakeoffEnum
};

typedef struct LaunchCmd{
    unsigned char msgType; // according to MessageTypeEnum
    unsigned char msgId;   // Incremental
    unsigned short missionId;
    unsigned char cs;
} LaunchCmd;


typedef struct FireFlyStatus{
	unsigned char msgType;
    unsigned char msgId;
    unsigned char isReadyToLaunch; // 0 means NOT ready
    unsigned char batteryPercentage;
    unsigned int BITStatus; // bitmapped DWord according to the table
    unsigned char cs;
} FireFlyStatus;


typedef struct SecretCmd {
	unsigned char msgType; // according to MessageTypeEnum
	unsigned char secret1[MAX_SECRET_SIZE];
	unsigned char secret2[MAX_SECRET_SIZE];
	unsigned char cs;
} SecretCmd;

typedef struct {
    int mission_number;
    int priority;
} Mission;

#endif /* APPLICATION_USER_ICD_ICD_H_ */
