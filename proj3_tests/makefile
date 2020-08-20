all:

thread:
	g++ -g -o threads.o -c ../Thread.cpp -m32
	g++ -g -o sem.o -c ../Semaphore.cpp -m32

test:
	g++ -g -o threads.o -c ../Thread.cpp -m32
	g++ -g -o sem.o -c ../Semaphore.cpp -m32
	g++ -o $(name)  tests/$(name).c ../ threads.o ../sem.o -m32

both: thread test

clean:
	rm -rf *.o app
	rm $(name)

clean_test:
	rm $(name)
