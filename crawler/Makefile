# Makefile for 'bag' module
#
# David Kotz, April 2016, 2017, 2019, 2021
# updated by Xia Zhou, July 2016, 2018, January 2019

LIBS = ../common/common.a ../libcs50/libcs50-given.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../libcs50 -I../common
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

crawler: crawler.o $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

crawler.o: crawler.c ../libcs50/hashtable.h ../libcs50/webpage.h ../libcs50/bag.h ../libcs50/mem.h ../libcs50/file.h ../libcs50/hash.h ../libcs50/counters.h

.PHONY: test valgrind clean

# expects a file `test.names` to exist; it can contain any text.
test: testing.out
	./testing.sh > testing.out

# expects a file `test.names` to exist; it can contain any text.
valgrind: testing.out
	$(VALGRIND) ./testing.sh > testing.out

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f crawler
	rm -f core
	rm ~/cs50-dev/tse-NiKKrope/common/.crawler
	rm ~/cs50-dev/tse-NiKKrope/common/{1..100} 
#   ^^  (for the crawler files) ^^