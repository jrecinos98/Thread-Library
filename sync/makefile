all:
	g++ -g -o thr.o -c Thread.cpp -m32
	g++ -g -o sem.o -c Semaphore.cpp -m32
	ld -m elf_i386 -r thr.o sem.o -o threads.o 
	rm thr.o sem.o

thread:
	g++ -c -o threads.o Thread.cpp -m32
	g++ -g -o sem.o -c Semaphore.cpp -m32
	ld -m elf_i386 -r thr.o sem.o -o threads.o 
	rm thr.o sem.o


test:
	g++ -o $(name)  tests/$(name).c threads.o -m32

both: thread test

clean:
	rm threads.o
	rm $(name)

clean_test:
	rm $(name)

clean:
	rm -rf *.o app
