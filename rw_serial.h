#include "serial.h"
#include "seek.h"

#define DELAY 100000

#define DISK_SIZE 10
long disk[DISK_SIZE];

elevator_t* elevator;

serial_t* serializer;
queue_t* waiting_q;
crowd_t* readers_crowd;
crowd_t* writers_crowd;

void create();

void *read_func(void *req);
void *write_func(void *req);
