#ifndef T_CONTROL
#define T_CONTROL


#include <stdlib.h>		// for exit() 
#include <stdio.h> 		// standard buffered input/output 
#include <setjmp.h> 	// for performing non-local gotos with setjmp/longjmp 


#define MAX_THREADS 129
#define STACK_SIZE 32767


static sigset_t mask;

//Contains info aboout a Thread
struct TCB{
	jmp_buf context;
	void *stack;

	//return value of thread
	void** ret_value;

	int threadID;

	//Set to 1 when running. Set to 0 if not initialized. Set to -1 if ready to run. 
	int state;

	//Set to 1 when exit happens;
	int exit;
	
	//Set to 1 when blocked
	int blocked;

};

//Contains all the threads and their info
struct TCBT{
	//One more to store the original main thread
	struct TCB TCBs[MAX_THREADS];
	int currentThread;
	int last;
};


//Blocks signals from SIGALRM
static void lock(){
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if(sigprocmask(SIG_BLOCK,&mask, NULL) < 0){
        perror("sigprocmask");
        exit(1);
    }
}

//Unblocks the signal.
static void unlock(){
	if(sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0){
        perror("sigprocmask");
        exit(1);
    }
}

#endif
