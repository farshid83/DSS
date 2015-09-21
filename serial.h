/*
 * serial.h
 */

#include <pthread.h>

typedef struct
{
	pthread_t id;
	void* next; //Linked List Queue
	//void* head; //head is not needed, q points to the head.
	pthread_cond_t cond;
} queue_t;

typedef struct
{
	pthread_t id;
	//int index;
	void* next; //Linked List Queue
	//void* head;
} crowd_t;

typedef struct
{
	int qindex;
	int rcindex;
	int wcindex;
	crowd_t* rchead;
	crowd_t* wchead;
	crowd_t* rctail;
	crowd_t* wctail;
	int sw;//0:readers crowd, 1:writers crowd
	queue_t* qhead;
	queue_t* qtail;
	int serial_number;
} serial_t;

#define cond_t int
int serializers;
pthread_mutex_t mutex;// = PTHREAD_MUTEX_INITIALIZER;

serial_t* Create_Serial();
void Serial_Enter(serial_t*);
void Serial_Exit(serial_t*);
queue_t* Create_Queue(serial_t*);
crowd_t* Create_Crowd(serial_t*);
cond_t Queue_Empty(serial_t*, queue_t*);
cond_t Crowd_Empty(serial_t*, crowd_t*);
void Serial_Enqueue(serial_t*, queue_t*, cond_t (*func)());
void Serial_Join_Crowd(serial_t*, crowd_t*, void* (*func)());
// void Serial_Dequeue(serial_t, queue_t*, cond_t ());
// void suspend();
// void resume();
