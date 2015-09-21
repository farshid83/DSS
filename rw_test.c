/*
 * Reader and Writer's Problem for disk scheduler
 * CSE 511 Test Program
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "rw_serial.h"

#define NUM_OF_REQUESTS 2

int main(int argc, char *argv[])
{
	// Init vars
	int i;
	int rc;
	srand(time(NULL));
	pthread_t reader[NUM_OF_REQUESTS];

	// Create the disk and serializer
	create();

	// Create the r/w requests (as threads) to the disk
	for (i = 0; i < NUM_OF_REQUESTS; i++)
	{
		request_t* req = malloc(sizeof(request_t));
		req->id = i;
		req->cylinder_num = rand() % DISK_SIZE;
		req->type = 0;
		rc = pthread_create(&reader[i], NULL, read_func, (void *)req);
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}
	printf("Main: disk requests created successfully\n");

	// Loop forever
	while(1);

	// Finish
	return 0;
}
