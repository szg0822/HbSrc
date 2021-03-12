flag = 1 
ifdef flag
#INCLUDES = -I/usr/local/arm/opencv-install/include -I.
#LIBDIRS = -L/usr/local/arm/opencv-install/lib
LIBDIRS = -L .
INCLUDES = -I .

CC=aarch64-linux-gnu-g++
GCC=aarch64-linux-gnu-gcc

else
CC=g++
GCC=gcc
INCLUDES = -I/usr/local/include
LIBDIRS = -L/usr/local/lib
endif

LIBS = -l LinuxLog
#LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui 
OPT = -O3 -g  -pthread -std=c++11 -g 		
#-Wall
OBJS= main.o MyUDP.o UdpFunc.o thread.o 
all:demo
		echo all:make complete
%.o:%.cpp
		$(CC) -c $(INCLUDES) $+ $(OPT) 
%.o:%.c
		$(GCC) -c $(INCLUDES) $+ 
demo:$(OBJS)
		$(CC) $(LIBDIRS)  -o $@ $+ $(OPT) $(LIBS)

.PHONY: all clean
clean:
		rm -rf *.o *~ demo 


