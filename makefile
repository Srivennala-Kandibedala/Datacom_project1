CC=gcc
CFLAGS=-g -std=gnu99 -Wall -pedantic

.SUFFIXES: .x

.PHONY : clean

test: chatclient chatserver

%.x : %.c
	$(CC) $(CFLAGS) -o $@ $<
%.x : %.cpps
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f chatserver chatclient
