##############################################################################
#
# Applications Makefile
#
##############################################################################

TARGET = writer

OBJECTS = writer.o

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

writer.o: writer.c
	$(CC) $(CFLAGS) -c writer.c

.PHONY: clean distclean mrproper
clean distclean mrproper:
	-rm -f *.o
	-rm -f $(TARGET)
