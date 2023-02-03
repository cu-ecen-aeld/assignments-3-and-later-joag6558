##############################################################################
#
# Socket server Makefile
#
##############################################################################

TARGET ?= aesd-circular-buffer

OBJECTS ?= aesd-circular-buffer.o

INCLUDES ?= -I.

LDFLAGS ?=

CFLAGS ?= -g -Wall -Werror 

#LIBS ?= -lm -lpthread -lrt
LIBS ?= -pthread -lrt

CC ?= $(CROSS_COMPILE)gcc

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET) $(OBJECTS)

aesd-circular-buffer.o: aesd-circular-buffer.c
	$(CC) $(CFLAGS) $(INCLUDES) -c aesd-circular-buffer.c

.PHONY: clean distclean mrproper
clean distclean mrproper:
	-rm -f *.o
	-rm -f $(TARGET)
