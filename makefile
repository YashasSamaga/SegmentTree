BUILDDIR = build
EXECUTABLE = $(BUILDDIR)/a.out

CC=g++
CCFLAGS=-std=c++17
CXXSOURCES=main.cpp

.PHONY: all

all: build exec

build: $(EXECUTABLE)

exec:
	$(EXECUTABLE)

clean:
	rm $(EXECUTABLE)

$(EXECUTABLE) : $(CXXSOURCES)
	$(CC) $(CCFLAGS) $(CXXSOURCES) -o $(EXECUTABLE) 


