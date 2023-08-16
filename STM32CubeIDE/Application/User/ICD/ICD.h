/*
 * ICD.h
 *
 *  Created on: Aug 15, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_ICD_ICD_H_
#define APPLICATION_USER_ICD_ICD_H_

void ICD_handle(void *args);

typedef struct RequestMessage{
    unsigned char msgType;
    unsigned char requestedType;
    unsigned char msgSize;
} RequestMessage;



typedef struct NavFrameINS{
    unsigned char msgType; // according to MessageTypeEnum
    unsigned char msgId;   // Incremental
    double INS_TimeTagSec; // GPS time from GNSS Week + Seconds from week start ( Seconds from Jan 6 1980 no leap), should be UTC ? else ?
    double PositionLatitudeGeoRad;
    double PositionLongitudeGeoRad;
    float PositionAltitudeMeterAEL; // should be ASL ?
    float VelocityNorthMeterPerSec;
    float VelocityEastMeterPerSec;
    float VelocityDownMeterPerSec;
    float AzimuthDeg;      // 0 means North,  180 and -180 means South
    float PitchDeg;        // Right-handed rotation from local level around xaxis in degrees
    float RollDeg;         // Right-handed rotation from local level around yaxis in degrees
    unsigned char isValid; // 0 means not valid else is valid
    unsigned int spare;

} NavFrameINS;

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
    unsigned char altitudeType; // According to AltitudeTypeEnum
    float takeoffAltitude;
    unsigned char endOfMissionActivity; // According to EndOfMissionActivityEnum
    unsigned char cs;
} LaunchCmd;


typedef struct fireFlyStatus{
    unsigned char msgId;
    unsigned char isReadyToLaunch; // 0 means NOT ready
    unsigned char batteryPercentage;
    unsigned int BITStatus; // bitmapped DWord according to the table
    unsigned char cs;
} fireFlyStatus;



#endif /* APPLICATION_USER_ICD_ICD_H_ */
