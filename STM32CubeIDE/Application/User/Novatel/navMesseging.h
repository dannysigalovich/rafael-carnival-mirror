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

void init_CPT7_broker();


enum INS_Status {
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
};


typedef struct {
	// TODO: header don't really understood
	ulong week;
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
	enum INS_Status status;
	uint32_t crc;
}INSPVA;


#endif /* NAVMESSEGING_H_ */
