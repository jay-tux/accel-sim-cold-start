CC=g++
CXXFLAGS=-Wall -Wextra -O3 -std=c++2a

all: reuse footprint mem_access mem_controller

%: %.cpp Makefile
	$(CC) $(CXXFLAGS) -o $@ $<


clean:
	rm -f reuse footprint mem_access mem_controller

.PHONY: clean
