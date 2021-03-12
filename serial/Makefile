
CC=g++
GCC=gcc
INCLUDES = -I/usr/local/include
LIBDIRS = -L/usr/local/lib

OPT = -O3 -g -Wall -pthread -std=c++11 -g 
OBJS= main.o Serial.o 
all:serial 
		echo all:make complete
serial:main.cpp
	$(CC) main.cpp Serial.cpp -o $@ 
clean:
	-rm -rf serial 

