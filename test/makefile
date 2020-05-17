CC = gcc
CFLAGS  = -g -Wall -I..\include

default: test_edm

test_edm:  test_edm.o ucxh_edm.o
	$(CC) $(CFLAGS) -o test_edm test_edm.o ucxh_edm.o

test_edm.o:  test_edm.c ..\include\ucxh_edm.h
	$(CC) $(CFLAGS) -c test_edm.c

ucxh_edm.o:  ..\src\ucxh_edm.c ..\include\ucxh_edm.h
	$(CC) $(CFLAGS) -c ..\src\ucxh_edm.c -o ucxh_edm.o
	
