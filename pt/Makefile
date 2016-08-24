#
# To build programs...
# Author: Benigno
#

USBDEVICE = ATTINY45_DEVICE

CC     = g++
#CFLAGS = -O -DMS5611_DEVICE -D$(USBDEVICE) -std=c++11
CFLAGS = -O -DMS5611_DEVICE -D$(USBDEVICE)
LIBS   = -lusb


all: test loop


libMyUSB.a:	init.o Attiny45.o MS56xx.o ADT7420.o 
	ar rvu libMyUSB.a init.o Attiny45.o MS56xx.o ADT7420.o

test:	test.cc init.h libMyUSB.a
	${CC} ${CFLAGS} -o test test.cc -L . -lMyUSB ${LIBS}

loop:	loop.cc init.h libMyUSB.a
	${CC} ${CFLAGS} -o loop loop.cc -L . -lMyUSB ${LIBS}

init.o: init.cc init.h Attiny45.h MS56xx.h ADT7420.h
	${CC} ${CFLAGS} -c init.cc

Attiny45.o: Attiny45.cc Attiny45.h
	${CC} ${CFLAGS} -c Attiny45.cc

MS56xx.o: MS56xx.cc MS56xx.h
	${CC} ${CFLAGS} -c MS56xx.cc

ADT7420.o: ADT7420.cc ADT7420.h
	${CC} ${CFLAGS} -c ADT7420.cc

clean: 
	  rm -f *.o libMyUSB.a *~ test loop





