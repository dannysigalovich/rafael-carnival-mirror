/*
 * navMesseging.h
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#ifndef NAVMESSEGING_H_
#define NAVMESSEGING_H_

#include <stdint.h>

#define CPT7_BROKER_THREAD_PRIO    ( osPriorityAboveNormal )

void init_novatel_debug();

typedef enum INSType{
	INSPVAType = 507,
	INSPVAXType = 1465,
	INSSTDType = 2051,
} INSType;


typedef struct {
	uint8_t sync[3]; // Hexadecimal 0xAA, 0x44, 0x12
	uint8_t headLength;
	unsigned short msgID;
	uint8_t msgType;
	uint8_t port;
	unsigned short  msgLength; // The length in bytes of the body of the message, not including the header nor the CRC
	unsigned short seq;
	uint8_t idleTime;
	uint8_t timeStatus;
	unsigned short week;
	uint32_t ms;
	unsigned long recevierStatus;
	unsigned short reserved;
	unsigned short recevierSWVersion;
} INS_header;

typedef struct {
	INS_header header;
	unsigned long week;
	double seconds;
	double latitude;
	double longitude;
	double height;
	double northVelocity;
	double eastVelocity;
	double upVelocity;
	double roll;
	double pitch;
	double azimuth;
	uint32_t status;
	uint32_t crc;
} INSPVA;


typedef struct INSSTDEV{
	INS_header header;
	float latitude_sigma;             // Latitude standard deviation (m)
	float longitude_sigma;            // Longitude standard deviation (m)
	float height_sigma;               // Height standard deviation (m)
	float north_velocity_sigma;       // North velocity standard deviation (m/s)
	float east_velocity_sigma;        // East velocity standard deviation (m/s)
	float up_velocity_sigma;          // Up velocity standard deviation (m/s)
	float roll_sigma;                 // Roll standard deviation (degrees)
	float pitch_sigma;                // Pitch standard deviation (degrees)
	float azimuth_sigma;              // Azimuth standard deviation (degrees)
	unsigned long ext_sol_status;     // Extended solution status (Ulong)
	unsigned short time_since_update; // Elapsed time since last update (seconds, Ushort)
	unsigned short reserved_1;        // Reserved (Ushort)
	unsigned long reserved_2;         // Reserved (Ulong)
	unsigned long reserved_3;         // Reserved (Ulong)
	uint32_t crc;                 // 32-bit CRC (Hex)
} INSSTDEV;



typedef struct __attribute__((packed, aligned(1))) INSPVAX{
	INS_header header;
	uint32_t status;
	uint32_t posType;
    double latitude;
    double longitude;
    double height;
    float undulation;
    double northVelocity;
    double eastVelocity;
    double upVelocity;
    double roll;
    double pitch;
    double azimuth;
    float latStdDev;
    float longStdDev;
    float heightStdDev;
    float northVelStdDev;
    float eastVelStdDev;
    float upVelStdDev;
    float rollStdDev;
    float pitchStdDev;
    float azimuthStdDev;
	uint32_t extSolutionStatus;
    unsigned short timeSinceUpdate;
    uint32_t crc;                 // 32-bit CRC (Hex)
} INSPVAX;



#endif /* NAVMESSEGING_H_ */
