# Thread Library 
- **Description** : 
For this project I implemented a new method on my thread library called pthread_join. 
This method allows another thread to obtain the return value from the running thread. 
Furthermore I implemented a lock() and unlock() method to block signals and use it when 
calling semaphore methods to prevent it from being interrupted as they are meant to be atomic functions. 
I also implemented semaphore methods and my own semaphore struture to allow for more flexibility and safety 
when working with threads and critical sections. The most dificult part was figuring out how to use my own 
semaphore alongside sem_t as I did not know how to properly o it. Also troubling was handling the return 
value from pthread_join as I was getting strange values.
___
- **Relevant Area** : Data Structures
___
- **Tools / Platforms**:  C++, Unix
___