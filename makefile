all:
	g++ -g -o thr.o -c Thread.cpp -m32
	g++ -g -o sem.o -c Semaphore.cpp -m32
	ld -m elf_i386 -r thr.o sem.o -o threads.o 
	rm *.o
	g++ -o app threads.o proj3_tests/tests/basic_sem_3.c -m32
clean:
	rm -rf *.o app
