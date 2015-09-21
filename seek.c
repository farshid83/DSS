/*
 * seek.c
 *
 *  Created on: Aug 30, 2015
 *      Author: dxz149
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#include "seek.h"

pthread_mutex_t mutex;// = PTHREAD_MUTEX_INITIALIZER;

elevator_t* Create_Elevator()
{
	// Init buffer
	int i;
	for(i = 0; i < MAX_BUFFER_SIZE; i++)
		buffer[i] = -1;
	// Init elevator
	elevator_t* elevator = malloc(sizeof(elevator_t));
	elevator->direction = 1;
	elevator->CurrentCylinder = 0;
	elevator->LastIndex = 0;
	elevator->active_size = 0;
	return elevator;
}

void Elevator_Enter(elevator_t* elevator, request_t* req)
{
	//int rc = pthread_mutex_lock(&mutex);
	int i = 0;
	while ( (i < MAX_BUFFER_SIZE) && (req->cylinder_num > buffer[i]) && (buffer[i] != -1))
		i++;
	//printf("cyl = %ld, i = %ld, buf[i] = %ld\n",req->cylinder_num, i, buffer[i]);
	if (i == MAX_BUFFER_SIZE)
		printf("Oh! Max Buffer size!\n");
	else if (buffer[i] == -1)
		buffer[i] = req->cylinder_num;
	else
	{ // req->cylinder_num <= buffer[i]
		int j = MAX_BUFFER_SIZE - 2;
		while (j > i)
		{
			if(buffer[j] != -1)
				buffer[j+1] = buffer[j];
			j--;
		}
		buffer[i+1] = buffer[i];
		buffer[i] = req->cylinder_num;
	}
	//rc = pthread_mutex_unlock(&mutex);
}

void Elevator_Exit(elevator_t* elevator, request_t* req)
{
	while(Elevator_Status(elevator) != req->cylinder_num);
	//int rc = pthread_mutex_lock(&mutex);
	if(elevator->direction == 1)
	{
		int i = elevator->LastIndex;
		while ( (i < MAX_BUFFER_SIZE - 1) && (buffer[i] != -1) )
		{
			buffer[i] = buffer[i+1];
			i++;
		}
		if (i == MAX_BUFFER_SIZE - 1)
			buffer[i] = -1;
		//LastIndex no change
		elevator->CurrentCylinder = buffer[elevator->LastIndex];
	}
	else if (elevator->direction == -1)
	{
		int i = elevator->LastIndex;
		while ( (i < MAX_BUFFER_SIZE - 1) && (buffer[i] != -1) )
		{
			buffer[i] = buffer[i+1];
			i++;
		}
		if (i == MAX_BUFFER_SIZE - 1)
			buffer[i] = -1;
		elevator->LastIndex--;
		if (elevator->LastIndex < 0)
		{
			elevator->LastIndex = 0;
			elevator->direction = 1;
		}
		elevator->CurrentCylinder = buffer[elevator->LastIndex];
	}
	//rc = pthread_mutex_unlock(&mutex);
//	11 12 15 19 22 34 -1 -1
//	0  1  2  3  4  5  6  7
}

condition Elevator_Status(elevator_t* elevator)
{
	return elevator->CurrentCylinder;
}
