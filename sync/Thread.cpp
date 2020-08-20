#include "Thread.h"

static int scheduler_init;
struct TCBT Table;
static int joined[MAX_THREADS]; 

static struct itimerval it_val;
static struct sigaction act;
static int count=0;
static unsigned int res;

static int ptr_mangle(int p)
{
    unsigned int ret;
    asm(" movl %1, %%eax;\n"
        " xorl %%gs:0x18, %%eax;"
        " roll $0x9, %%eax;"
        " movl %%eax, %0;"
    : "=r"(ret)
    : "r"(p)
    : "%eax"
    );
    return ret;
}

int pthread_create(pthread_t *restrict_thread, const pthread_attr_t *restrict_attr, 
                    void *(*start_routine)(void*), void *restrict_arg){
    //if thread subsys not initialized
    if(scheduler_init == 0){
        //Main will be thread 0
        int tID= MAIN_ID;

        //Initialized joined thread arrays
        for(int i=0; i< MAX_THREADS; i++){
            joined[i]=-1;
        }

        struct TCB *mainTCB= & Table.TCBs[tID];
        //save main in a TCB
        setjmp(mainTCB->context);
        //guaranteed to be 0
        mainTCB->threadID= tID;
        //Initially it will be the one running. Alarm_handler will change this
        mainTCB->state=1;
        //Main not initialy blocked
        mainTCB->blocked=0;
        //Main hasn't exit.
        mainTCB->exit=0;
        //start timer.
        initScheduler();
       

    }
    //Find the next available spot
    int tID= emptySlot();
    
    struct TCB *newTCB= &Table.TCBs[tID];
    //Returns the thread ID to the caller
    *restrict_thread= tID;
    //Allocate memory for the stack
    char *stack= (char *) malloc(STACK_SIZE);

    //points to the location in the stack where the argument is found
    int *address_ptr= (int*) (stack + STACK_SIZE - 4);
    *address_ptr= (int) restrict_arg;

    //points to the location in the stack where the function reference is found
    int * exit_ptr= (int*) (stack + STACK_SIZE - 8);
    *exit_ptr= (int) pthread_exit_wrapper;

    //initialize TCB
    newTCB->stack= stack;
    newTCB->threadID= tID;
    newTCB->state=-1;
    newTCB->blocked=0;
    newTCB->exit=0;
    newTCB->ret_value= (void**)malloc(sizeof(void*));

    //create context for the thread
    setjmp(newTCB->context);

    //Modify stack pointer to point to stack pointer
    newTCB->context->__jmpbuf[SP]= ptr_mangle((int)(stack + STACK_SIZE-8));
    //Modify PC to hold start routine
    newTCB->context->__jmpbuf[PC]= ptr_mangle((int)start_routine);

    Table.last= tID;
    

       
    return 0;                        
}

//Returns the thread ID
pthread_t pthread_self(void){
    //Thread ID is the index so the index of current running thread is the index
    return Table.currentThread;

}

int pthread_join(pthread_t thread, void **value_ptr){
    //obtain thread specified by parameter thread
    struct TCB *other = &Table.TCBs[thread];
    //If the specified thread has not finished wait for it and block current. Else return immediately
    if(other->exit != 1){
         //Link the thread with the joined threads. The given thread will then be able to unblock the calling thread
        joined[thread]= pthread_self();
        
        //printf("Blocked thread: %d\n", pthread_self());
        //printf("Thread to join: %d\n", thread);
        
        struct TCB *current = &Table.TCBs[Table.currentThread];
        current->blocked=1;
        while(current->blocked == 1){
            //Wait until the other thread     
        }

    }
    *value_ptr= *other->ret_value;
    return 0;

}

//Called when a thread terminates .
void pthread_exit(void *value_ptr){
    lock();

    Table.TCBs[joined[pthread_self()]].blocked=0;
    
    //printf("Unblocked: %d\n", joined[pthread_self()]);
    
    struct TCB *t= & Table.TCBs[Table.currentThread];
    *t->ret_value=  value_ptr;
    
    //If the current thread is main handle special case.
    if( Table.currentThread == MAIN_ID){
        while(isThreadActive()){
            //wait for threads to finish
        }
        exit(0);
    }

    //reset state
    t->state=0;
    t->exit=1;

    //Free the allocated memory from heap
    if(t->stack){
        if(t->threadID != Table.last){
            free(t->stack);
        }
        else{
            void* stack= realloc(t->stack,1);
            free(stack);
        }
   
    }
    //Pause because thread doesn't know where to return to.
    unlock();
    sleep(100);
    exit(0);

}

//Passes return value to pthread_exit
void pthread_exit_wrapper()
{
  asm("movl %%eax, %0\n" :"=r"(res)); 
  pause();
  pthread_exit((void *) res);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions as the scheduler and switches between threads
void alarm_handler(int sig){
    //printf("Current Thread: %d\n", pthread_self());
    //(LOCK)
    lock();
    struct TCB *current = & Table.TCBs[Table.currentThread];


    //Get the next thread to be executed. Modifies the currentThread variable. 
    //Must be called before saveContext because the state is modified to -1 and it loops forever
    int next= setNext();

    if(current->state == 1){
       saveContext(current);
    } 

    struct TCB *nextThread= &Table.TCBs[next];

    //(UNLOCK)

    //Restore the next thread thread
    unlock();
    restoreContext(nextThread);

}

void saveContext(struct TCB *c){
    //-1 means it is waiting to run
    c->state=-1;
    if(setjmp(c->context)){
        return ;
    }
   
}

void restoreContext(struct TCB* n){
    //1 means that it is the running thread
    n->state=1;
    longjmp(n->context, 1);
}

//Initializes the timer
void initScheduler(){
    scheduler_init=1;

    /*on signal, call singal_handler function*/
    act.sa_handler = alarm_handler;
    // we want to make sure that we intercept signals even when we're inside the signal_handler function
    act.sa_flags= SA_NODEFER;
    if(sigaction(SIGALRM, &act, NULL)== -1){
        perror("Unable to catch SIGALRM");
        exit(1);
    }

    //seconds
    it_val.it_value.tv_sec = INTERVAL/1000;
    //microseconds
    it_val.it_value.tv_usec=  (INTERVAL*1000) % 1000000;

    //Next timer should use the same time interval
    it_val. it_interval= it_val.it_value;

    //Start timer
    if(setitimer(ITIMER_REAL, &it_val, NULL) == -1){
        perror("Error starting timer");
        exit(1);
    }
}

//Goes through the PCB and finds the first empty slot that hasn't been initialized or has exited
int emptySlot(){
    //Find the next empty slot in the array 
    for(int i=0; i < MAX_THREADS; ++i){
        //If a thread has not exited or it has not been inititalized.
        if (Table.TCBs[i].exit == 0 && Table.TCBs[i].state == 0)
            return i;
    }
    return -1;

}

//Returns the ID of the thread that is scheduled to run next
int setNext(){
    //Without a counter, when there is only one thread it will loop forever
    int count=0;
    //Loop until a thread waiting for execution is found.
    while(!isReady(Table.currentThread) && count < MAX_THREADS){
        Table.currentThread= ((Table.currentThread + 1) % (MAX_THREADS));
        count++;
    }
    return Table.currentThread;
}

//Return 1 if the thread with the given id is waiting for execution
int isReady(int id){

    //Ready if it is waiting to run and it hasn't been blocked 
    return (Table.TCBs[id].state==-1 && Table.TCBs[id].blocked != 1);
}

//Pre-condition: will only be called by the main thread. (ACCESS GLOBAL VARIABLES SO LOCK)
int isThreadActive(){
    int count=0;
    while(count < MAX_THREADS){
        //if the state of the thread at the index is waiting and the thread is not the main thread
        if(Table.TCBs[(count % MAX_THREADS)].state == -1 && count !=0 ){
            return 1;
        }
        //If count exceeds MAX_THREADS then all threads haven't been initialized or 
        if(count >= MAX_THREADS){
            return 0;
        }
        count++;
    }
    return 1;
}



