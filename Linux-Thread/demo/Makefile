
CC            = gcc
RM            = rm -f

LDFLAGS = -s
CFLAGS  = -O2 -Wall
LIBS    = -lpthread

OBJECTS = main.o keyboard.o
PROGRAM = thread_test

all: $(PROGRAM)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROGRAM): $(OBJECTS)
	-$(RM) $@
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	-$(RM) $(OBJECTS) $(PROGRAM)
