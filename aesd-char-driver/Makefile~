##############################################################################
#
# Socket server Makefile
#
##############################################################################

TARGET ?= aesdsocket

OBJECTS ?= aesdsocket.o

INCLUDES ?= -I.

LDFLAGS ?=

CFLAGS ?= -g #-Wall -Werror 

#LIBS ?= -lm -lpthread -lrt
LIBS ?= -pthread -lrt

CC ?= $(CROSS_COMPILE)gcc

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET) $(OBJECTS)

aesdsocket.o: aesdsocket.c
	$(CC) $(CFLAGS) $(INCLUDES) -c aesdsocket.c

.PHONY: clean distclean mrproper
clean distclean mrproper:
	-rm -f *.o
	-rm -f $(TARGET)
