#
# To build programs...
# Author: Benigno
#

CC     = g++
CFLAGS = -O 
LIBS   = -L . -lN1471

all: test

libN1471.a:	N1471.o
	ar rvu libN1471.a N1471.o

test:	test.cc N1471.h libN1471.a 
	${CC} -o test test.cc ${LIBS}

N1471.o: N1471.cc N1471.h
	${CC} ${CFLAGS}	 -c N1471.cc

clean:
	rm -f *.o libN1471.a *~ test





