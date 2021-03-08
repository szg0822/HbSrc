
CC=g++
GCC=gcc
INCLUDES = -I/usr/local/include -I.
LIBDIRS = -L/usr/local/lib

OPT = -O3 -g -pthread
OBJS= LinuxLog.o MyFile.o MyThread.o MyIpcMQ.o MyIpcSocket.o 
all:demo
		echo all:make complete
%.o:%.cpp
		$(CC) -c $(INCLUDES) $+ $(OPT) 
%.o:%.c
		$(GCC) -c $(INCLUDES) $+ $(OPT) 
demo:$(OBJS)
		$(GCC) $(LIBDIRS)  -o $@ $+ $(OPT) 

.PHONY: all clean
clean:
		rm -rf *.o *~ demo 

