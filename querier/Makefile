#Makefile for querier

LIBS = ../common/common.a ../libcs50/libcs50-given.a

# uncomment the following to turn on verbose memory logging
TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../libcs50 -I../common
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

querier: querier.o $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

querier.o: querier.c ../libcs50/hashtable.h ../libcs50/webpage.h ../libcs50/bag.h ../libcs50/mem.h ../libcs50/file.h ../libcs50/hash.h ../libcs50/counters.h

.PHONY: test valgrind clean

test:
	bash testing.sh

queriertest.o: queriertest.c ../libcs50/hashtable.h ../libcs50/webpage.h ../libcs50/bag.h ../libcs50/mem.h ../libcs50/file.h ../libcs50/hash.h ../libcs50/counters.h

valgrind: testing.out
	$(VALGRIND) ./testing.sh > testing.out

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f querier
	rm -f queriertest
	rm -f core
	rm -f testing.out
	rm -f queriertest.o
	rm ~/cs50-dev/tse-NiKKrope/common/testing.out
#   ^^  (for the querier files) ^^