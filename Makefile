#
# To build programs...
# Author: Benigno
#

CC       = g++
CFLAGS   = -O 
LIBS     = -Lpt/ -lMyUSB -Ln1471/ -lN1471 -lusb
DIRS     = pt n1471
INCLUDES = -Ipt/ -In1471/

all: libs vreg

vreg:	main.cc constants.h n1471/N1471.h pt/init.h n1471/libN1471.a pt/libMyUSB.a
	${CC} ${CFLAGS} ${INCLUDES} -o vreg main.cc ${LIBS}

libs:
	for d in $(DIRS); do ( cd $$d; $(MAKE) ); done 

clean: 
	rm -f *.o *~ vreg
	for d in $(DIRS); do ( cd $$d; $(MAKE) clean ); done





