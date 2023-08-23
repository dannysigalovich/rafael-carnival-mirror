/*
 * navMesseging.h
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#ifndef NAVMESSEGING_H_
#define NAVMESSEGING_H_

#define TCPSERVER_THREAD_PRIO    ( osPriorityAboveNormal )
#define CPT7_BROKER_THREAD_PRIO    ( osPriorityAboveNormal )

#include <stdint.h>

void init_CPT7_broker();


typedef enum INSType{
	INSPVAType = 507,
	INSSTDType = 2051
} INSType;


typedef enum{
	INS_INACTIVE = 0,
	INS_ALIGNING,
	INS_HIGH_VARIANCE,
	INS_SOLUTION_GOOD,
	INS_SOLUTION_FREE = 6,
	INS_ALIGNMENT_COMPLETE,
	DETERMINING_ORIENTATION,
	WAITING_INITIALPOS,
	WAITING_AZIMUTH,
	INITIALIZING_BIASES,
	MOTION_DETECT,
	WAITING_ALIGNMENTORIENTATION
} INS_Status;


typedef enum {
    TIME_UNKNOWN = 20,
    TIME_APPROXIMATE = 60,
    TIME_COARSE_ADJUSTING = 80,
    TIME_COARSE = 100,
    TIME_COARSE_STEERING = 120,
    TIME_FREEWHEELING = 130,
    TIME_FINE_ADJUSTING = 140,
    TIME_FINE = 160,
    TIME_FINE_BACKUP_STEERING = 170,
    TIME_FINE_STEERING = 180,
    TIME_SATTIME = 200
} TimeStatus;


typedef struct {
	uint8_t sync[3]; // Hexadecimal 0xAA, 0x44, 0x12
	uint8_t headLength;
	unsigned short msgID;
	uint8_t msgType;
	uint8_t port;
	uint8_t msgLength; // The length in bytes of the body of the message, not including the header nor the CRC
	unsigned short seq;
	uint8_t idleTime;
	TimeStatus timeStatus;
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
	INS_Status status;
	unsigned int crc;
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
	unsigned int crc;                 // 32-bit CRC (Hex)
} INSSTDEV;





void printINSPVAFields(const INSPVA *inspva);

#endif /* NAVMESSEGING_H_ */
