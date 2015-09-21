/*
 * Elevator-based disk serializer
 */

#include "rw_serial.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


long* read_data()
{
	int key = rand() % DISK_SIZE;
	usleep(rand()%10*DELAY);
	return (long *) disk[key];
}

void* write_data()
{
	int key = rand() % DISK_SIZE;
	usleep(rand()%10*DELAY);
	disk[key] = key;
	return NULL;
}

cond_t read_queue_cond()
{
	return Crowd_Empty(serializer, writers_crowd);
}

cond_t write_queue_cond()
{
	return (Crowd_Empty(serializer, readers_crowd) &&
		Crowd_Empty(serializer, writers_crowd));
}

void create()
{
	// Init disk
	int i;
	srand(time(NULL));
	for (i = 0; i < DISK_SIZE; i++)
		disk[i] = rand();
	// Init Elevator
	elevator = Create_Elevator();
	// Init Serializer
	serializer = Create_Serial();
	waiting_q = Create_Queue(serializer);
	readers_crowd = Create_Crowd(serializer);
	writers_crowd = Create_Crowd(serializer);
}

void *read_func(void *req)
{
	request_t* request = req;
/*	Elevator_Enter(elevator, request);
	printf("Request #%ld entered the elevator to read at %ld\n", request->id, request->cylinder_num);
	Elevator_Exit (elevator, request);
	printf("Request #%ld exited the elevator to read at %ld\n", request->id, request->cylinder_num);
	Serial_Enter(serializer);
	printf("Request #%ld started reading at %ld\n", request->id, request->cylinder_num);
	Serial_Enqueue(serializer, waiting_q, &read_queue_cond);
	Serial_Join_Crowd(serializer, readers_crowd,(void *) (&read_data));
	Serial_Exit(serializer);
	printf("Request #%ld finished reading at %ld\n", request->id, request->cylinder_num);
*/
	int j;
	for(j = 0; j < 1000; j++)
	{
		printf("%ld -- %d before\n", request->id, j);
		sleep(5);
		printf("%ld -- %d after \n", request->id, j);
		//pthread_yield();
		//sched_yield();
	}
	pthread_exit(NULL);
}

void *write_func(void *req) // id is the cylinder number
{
	request_t* request = req;
	Elevator_Enter(elevator, request);
	printf("Write #%ld entered the elevator to write at %ld\n", request->id, request->cylinder_num);
	Elevator_Exit (elevator, request);
	printf("Write #%ld exited the elevator to write at %ld\n", request->id, request->cylinder_num);
	Serial_Enter(serializer);
	printf("Write #%ld started writing at %ld\n", request->id, request->cylinder_num);
	Serial_Enqueue(serializer, waiting_q, &write_queue_cond);
	Serial_Join_Crowd(serializer, writers_crowd, &write_data);
	Serial_Exit(serializer);
	printf("Write #%ld finished writing at %ld\n", request->id, request->cylinder_num);
	pthread_exit(NULL);
}
