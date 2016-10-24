# Makefile de pong - 24/02/2012

SHELL  = /bin/bash
CC     = gcc
MKDEP  = gcc -MM
RM     = rm -f
CFLAGS = `pkg-config gtk+-2.0 --cflags` -W -Wall -ansi -pedantic -std=c89
LIBS   = `pkg-config gtk+-2.0 --libs`

OBJECTS = pong.o util.o info.o gui.o font.o
EXEC    = pong

.c.o :
	$(CC) $(CFLAGS) -c $*.c

all :: $(EXEC)

$(EXEC) : $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

clean ::
	$(RM) *.o $(EXEC)

depend ::
	$(MKDEP) *.c > .depend

include .depend

