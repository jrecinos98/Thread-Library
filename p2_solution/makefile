all:

thread:
	g++ -c -o threads.o ../Thread.c -m32

test:
	g++ -o $(name)  tests/$(name).c threads.o -m32

both: thread test

clean:
	rm threads.o
	rm $(name)

clean_test:
	rm $(name)
