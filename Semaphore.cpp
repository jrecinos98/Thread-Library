
#include "Semaphore.h"

int sem_init(sem_t *sem, int pshared, unsigned value){
	lock();
	struct my_sem* redefSem= (my_sem*) malloc(sizeof(struct my_sem));
	redefSem->value= value;
	redefSem->init=1;
	redefSem->waiting_threads = new std::queue<int>();//(std::queue<int>*) malloc(sizeof(std::queue<int>));
	sem->__align= (long int) redefSem;
	unlock();
	
	return 0;
}

int sem_destroy(sem_t *sem){
	lock();
	//Obtain address of my semaphore and then cast it back to a pointer
	struct my_sem* stored= (my_sem*) sem->__align;
	
	if(stored -> init){
		//Free the memory
		delete(stored->waiting_threads);
		free(stored);
		//Not needed but eh
		sem->__align=0;
	}
	sem= NULL;

	unlock();

	return 0;

}

int sem_wait(sem_t *sem){
	lock();
	//Obtain address of my semaphore and then cast it back to a pointer
	struct my_sem* stored= (my_sem*) sem->__align;
	//If it equals 0 then add the thread to the queue and make it wait
	if(stored->value == 0){
		stored->waiting_threads->push(Table.currentThread);	
		//printf("%s%d\n", "Stored Thread ID: ", stored->waiting_threads->front());

		//Block the current thread until there is space to access the critical section
		Table.TCBs[Table.currentThread].blocked=1;

		//Unock it so that a new thread can execute
		unlock();
		while(stored->value <= 0){

		}

		//Lock it again so it can't be interrupted
		lock();
	}

	stored->value-=1;

	//DO NOT UNLOCK. UNLOCK OCCURS IN sem_post()

	return 0;

}

int sem_post(sem_t *sem){
	//Obtain address of my semaphore and then cast it back to a pointer
	struct my_sem* stored= (my_sem*) sem->__align;
	

	//Increment counter
	stored->value+=1;

	if(!stored->waiting_threads->empty()){
		int tID= stored->waiting_threads->front();
		stored->waiting_threads->pop();
		Table.TCBs[tID].blocked=0;
	}

	unlock();
	return 0;

}
