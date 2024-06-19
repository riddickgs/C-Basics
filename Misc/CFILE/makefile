#This is sample makefile for random projects

CROSS=
GCC= $(CROSS)gcc
LIBS= -lrt
CFLAGS= -Wall -g -Werror -pthread


TARGET= thread bb c t1 t2 msg s2 tmr udp tt cmsg newtool
all: clean $(TARGET)

$(TARGET): 
	@$(GCC) $(CFLAGS) $@.c -o $@ $(LIBS)

clean:
	rm -rf $(TARGET)
