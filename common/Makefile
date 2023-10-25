# Makefile for 'bag' module
#
# David Kotz, April 2016, 2017, 2019, 2021
# updated by Xia Zhou, July 2016, 2018, January 2019

LIBS = common.a
OBJS = pagedir.o 

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

$(LIBS) : $(OBJS)
	ar cr $(LIBS) $(OBJS)

pagedir.o: pagedir.h

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f common.a
	rm -f core
	rm -f {1..100}
