#ifndef THREADS_H
#define THREADS_H

#define INTERVAL 50
#define MAX_THREADS 129
#define STACK_SIZE 32767
#define MAIN_ID 0
#define SP 4
#define PC 5

#include <stdlib.h>		// for exit() 
#include <stdio.h> 		// standard buffered input/output 
#include <setjmp.h> 	// for performing non-local gotos with setjmp/longjmp 
#include <sys/time.h> 	// for setitimer 
#include <pthread.h>    // for pthread_t definition
#include <signal.h>     // For sigaction
#include <unistd.h>
#include <semaphore.h>
#include <string.h>     //For memcpy

#include "Tcontrol.h"


//Used to encode the values inserted into stack pointer and program counter in jmp_buf
static int ptr_mangle(int p);

//Creates a new thread within a process. Allocates memory and creates ID
int pthread_create(pthread_t *restrict_thread, const pthread_attr_t *restrict_attr,
                   //start_routine is a pointer to a function of type void* (meaning it can be of anytype)
                   //and takes in as input an argument of type void*
                   void *(*start_routine)(void*), void *restrict_arg); 

//Terminates the calling thread. Puts exit code on value_ptr
void pthread_exit(void *value_ptr); 

//Wrapper function of pthread_exit. It obtains the return value of the function and passes it to pthread_exit
void pthread_exit_wrapper();

//Returns the thread ID of the calling thread
pthread_t pthread_self(void); 

//Blocks calling thread until joined.
int pthread_join(pthread_t thread, void **value_ptr);


//////////////////////////////////////////////////////////////////////////////////////////////////////////

//Handles context switching when a signal arrives
void alarm_handler(int);

//Finds the first empty slot in TCB table
int emptySlot();

//Returns the thread that is scheduled to run next;
int setNext();

//Will return 1 if the thread with the given ID is ready for execution
int isReady(int id);

//Returns 1 if another thread besides main is active.
int isThreadActive();

//Set up timer and sigaction
void initScheduler();

//Saves the context of the thread
void saveContext(struct TCB *c);

//restores a thread and longjmps
void restoreContext(struct TCB* n);



#endif