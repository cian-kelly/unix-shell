CC=gcc
CFLAGS=-I.
DEPS = functions.h
OBJ = main.o functions.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

assign2-20429616: main.o functions.o
	$(CC) -o assign2-20429616 main.o functions.o

clean:
	rm -rf *.o $(objects) assign2-20429616
