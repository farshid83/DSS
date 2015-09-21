/*
 * seek.c
 *
 *  Created on: Aug 30, 2015
 *      Author: dxz149
 */

// Disk scheduler (elevator-based)

#define condition long

#define MAX_BUFFER_SIZE 200
long buffer[MAX_BUFFER_SIZE]; // request buffer of disk scheduler

typedef struct
{
	long id; // request id
	long cylinder_num; // cylinder number
	int type; // read = 0 or write = 1
} request_t;

typedef struct
{
	int direction; // going up = 1 or down = -1
	long CurrentCylinder; // current cylinder
	long LastIndex;
	long active_size;
} elevator_t;

elevator_t* Create_Elevator();
//long SEEK(long);
void Elevator_Enter(elevator_t*, request_t*);
void Elevator_Exit (elevator_t*, request_t*);
condition Elevator_Status (elevator_t*);
