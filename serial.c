/*
 * disk_serializer.c
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

#include "serial.h"

/* prototype for thread routine */
void print_message_function ( void *ptr );

/* struct to hold data to be passed to a thread
   this shows how multiple data items can be passed to a thread */
typedef struct str_thdata
{
    int thread_no;
    char message[100];
} thdata;

int main2()
{
    pthread_t thread1, thread2;  /* thread variables */
    thdata data1, data2;         /* structs to be passed to threads */

    /* initialize data to pass to thread 1 */
    data1.thread_no = 1;
    strcpy(data1.message, "Hello!");

    /* initialize data to pass to thread 2 */
    data2.thread_no = 2;
    strcpy(data2.message, "Hi!");

    /* create threads 1 and 2 */
    pthread_create (&thread1, NULL, (void *) &print_message_function, (void *) &data1);
    pthread_create (&thread2, NULL, (void *) &print_message_function, (void *) &data2);

    /* Main block now waits for both threads to terminate, before it exits
       If main block exits, both threads exit, even if the threads have not
       finished their work */
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    /* exit */
    exit(0);
} /* main() */

/**
 * print_message_function is used as the start routine for the threads used
 * it accepts a void pointer
**/
void print_message_function ( void *ptr )
{
    thdata *data;
    data = (thdata *) ptr;  /* type cast to a pointer to thdata */

    /* do the work */
    printf("Thread %d says %s \n", data->thread_no, data->message);

    pthread_exit(0); /* exit */
} /* print_message_function ( void *ptr ) */
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
serial_t* Create_Serial()
{
	serial_t *s=malloc(sizeof(serial_t));
	s->qindex = 0;
	s->rcindex = 0;
	s->wcindex = 0;
	s->sw = 0;
	//s->serial_number=serializers;
	//serializers++;
	pthread_mutex_init(&mutex, NULL);
	//pthread_mutex_init(&qmutex, NULL);
	//s->crowd_queue=malloc(sizeof(crowd_t));
	//s->entry_queue=malloc(sizeof(crowd_t));
	return s;
}
/////////////////////////////////////////////////////////////////

queue_t* Create_Queue(serial_t* s)
{
	s->qhead = malloc(sizeof(queue_t));
	//q->index = 0;
	s->qhead->id = -1;//??? means no thread!
	s->qhead->next = NULL;
	s->qtail = NULL;
	//q->head=(void*)(q);
	pthread_cond_init(&(s->qhead->cond),NULL);
	return s->qhead;
}

/////////////////////////////////////////////////////////////////

crowd_t* Create_Crowd(serial_t* s)
{
	if(s->sw == 0)
	{
		s->rchead = malloc(sizeof(crowd_t));//malloc
		s->rchead->id = -1;
		s->rchead->next = NULL;
		s->rctail = NULL;
		s->sw = (s->sw + 1) % 2;//sw = 0 1 0 1 0 1 ...
		return s->rchead;
	}
	else
	{//s->sw == 1
		s->wchead = malloc(sizeof(crowd_t));//malloc
		s->wchead->id = -1;
		s->wchead->next = NULL;
		s->wctail = NULL;
		s->sw = (s->sw + 1) % 2;//s->sw = 0 1 0 1 0 1 ...
		return s->wchead;
	}
}
/////////////////////////////////////////////////////////////////

cond_t Queue_Empty(serial_t* s, queue_t* q)
{	//if ( s->qindex == 0) //(s->qhead == q) && (q->next == NULL) )//qindex == 0)// && (q->id == -1))
	//if((s->qhead == q) && (q->next == NULL))
	if((s->qhead == q) && (s->qindex == 0))
		return 1;
	else
		return 0;
}
/////////////////////////////////////////////////////////////////

cond_t Crowd_Empty(serial_t* s, crowd_t* c)
{
	//if ( ((s->rchead == c) || (s->wchead == c)) && (c->next == NULL) )//cindex == 0)// && (c->id == -1))
	if      ((s->rchead == c) && (s->rcindex == 0))
		return 1;
	else if ((s->wchead == c) && (s->wcindex == 0)) //cindex == 0)// && (c->id == -1))
		return 1;
	else
		return 0;
}
/////////////////////////////////////////////////////////////////

void Serial_Enter(serial_t* s)
{
	///////////////////////////////////////////////
	///////////////////////////////////////////////
	//LOCK
	int rc = pthread_mutex_lock(&mutex);
	//printf("Thread %lu Lock @ Serial_Enter = %d \n", pthread_self(), rc);
	///////////////////////////////////////////////
	///////////////////////////////////////////////
	//LOCKED
}
/////////////////////////////////////////////////////////////////

void Serial_Enqueue(serial_t* s, queue_t* q, cond_t (*func)())
{
	if(s->qhead!=q)
		exit(0);

	int rc;
	queue_t* element;
	pthread_t tid = pthread_self();

	element = malloc(sizeof(queue_t));
	if(s->qindex)
		//qindex > 0 or head < tail
		s->qtail->next = element;
	else
		//qindex == 0 or head == tail
		s->qhead->next = element;
	element->id = tid;
	element->next = NULL;
	pthread_cond_init(&(element->cond),NULL);
	s->qtail = element;
	s->qindex++;
	//printf("Thread %lu joins queue.\n", tid);

	element = q->next;//s->qhead->next
	if(element->id != tid) //Thread is in the middle of queue
		pthread_cond_wait(&(s->qtail->cond),&mutex);

	//UNLOCK
	rc = pthread_mutex_unlock(&mutex);
	//printf("Thread %lu Unlock @ Serial_Enqueue = %d \n", tid, rc);
	//UNLOCKED

	//Check condition until func()==1
	while(!func());
	//printf("Thread %lu is at head and Condition is true.\n",tid);

	//DEQUEUE
	rc = pthread_mutex_lock(&mutex);
	//printf("Thread %lu Lock @ Serial_Dequeue = %d \n", tid, rc);
	//printf("#Q = %d, #R = %d, #W = %d\n", s->qindex, s->rcindex, s->wcindex);
	element = q->next;//s->qhead->next
	q->next = element->next;//point to next element
	free((queue_t*)element);
	s->qindex--;
	element = q->next;
	if(element != NULL)
		pthread_cond_signal(&(element->cond));
	else
		s->qtail = NULL;
	//printf("#Q = %d, #R = %d, #W = %d\n", s->qindex, s->rcindex, s->wcindex);
	//rc = pthread_mutex_unlock(&mutex);
	//printf("Thread %lu Unlock @ Serial_Dequeue = %d \n", tid, rc);
	/////////
}
/////////////////////////////////////////////////////////////////

void Serial_Join_Crowd(serial_t* s, crowd_t* c, void* (*func)())
{
	//LOCK
	//int rc = pthread_mutex_lock(&mutex);
	pthread_t tid = pthread_self();//getppid
	//printf("Thread %lu Lock @ Serial_Join_Crowd = %d \n", tid, rc);
	//////
	crowd_t* element;
	if(s->rchead == c)
		s->sw = 0;
	else if(s->wchead == c)
		s->sw = 1;
	else
		exit(1);

	element = malloc(sizeof(crowd_t));
	if(s->sw)
	{
		if(s->wcindex)
			//qindex > 0 or head < tail
			s->wctail->next = element;
		else
			//qindex == 0 or head == tail
			s->wchead->next = element;
		element->id = tid;
		element->next = NULL;
		s->wctail = element;
		s->wcindex++;
	}
	else
	{
		if(s->rcindex)
			//qindex > 0 or head < tail
			s->rctail->next = element;
		else
			//qindex == 0 or head == tail
			s->rchead->next = element;
		element->id = tid;
		element->next = NULL;
		s->rctail = element;
		s->rcindex++;
	}

	//UNLOCK
	int rc = pthread_mutex_unlock(&mutex);
	//printf("Thread %lu Unlock @ Serial_Join_Crowd = %d \n", tid, rc);
	//UNLOCKED

	/////After Joining
	printf("#Q = %d, #R = %d, #W = %d\n", s->qindex, s->rcindex, s->wcindex);
	func();
	//////////////////

	//LEAVE CROWD
	rc = pthread_mutex_lock(&mutex);
	//printf("Thread %lu Lock @ Serial_Leave_Crowd = %d \n", tid, rc);

	element = c->next;//s->chead->next
	c->next = element->next;//point to next element
	free((crowd_t*)element);
	element = c->next;
	if(s->sw)
	{
		s->wcindex--;
		if(element == NULL)
			s->wctail = NULL;
	}
	else
	{
		s->rcindex--;
		if(element == NULL)
			s->rctail = NULL;
	}
	//printf("#Q = %d, #R = %d, #W = %d\n", s->qindex, s->rcindex, s->wcindex);
	/////////
}

/////////////////////////////////////////////////////////////////
void Serial_Exit(serial_t* s)
{
	//Exit Serializer
	int rc = pthread_mutex_unlock(&mutex);
	//printf("Thread %lu Unlock @ Serial_Exit = %d \n", pthread_self(), rc);
	///////////////////////////////////////////////
	///////////////////////////////////////////////
	//UNLOCKED
}
////////////////////////////////////////////////////////////////
