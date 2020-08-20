#ifndef SEMAPHORE_H

#define SEMAPHORE_H
#define SEM_VALUE_MAX 65536


#include <semaphore.h>
#include <sys/time.h> 	// for setitimer 
#include <signal.h>     // For sigaction
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>

#include "Tcontrol.h"


extern struct TCBT Table;


struct my_sem{
	int value;
	std::queue <int> *waiting_threads;
	int init;


};

//Initializes a semaphore to value .
int sem_init(sem_t *sem, int pshared, unsigned value);

int sem_destroy(sem_t *sem);

//Allows a thread to access the critical section
int sem_wait(sem_t *sem);
//Signals the critical section is available
int sem_post(sem_t *sem);

#endif