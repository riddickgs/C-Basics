#this is makefile 
#
TARGETS= call_thread one_thread count

CROSSTOOL=

GCC=$(CROSSTOOL)gcc

LIBS= -lrt
CFLAGS= -Wall -g -std=c99 -Werror -pthread $(LIBS)

all: clean $(TARGETS)

clean:
	rm -f $(TARGETS)

$(TARGETS):
	$(GCC) $(CFLAGS) $@.c -o $@

