CC=gcc 
CCFLAGS=-O3 

CPP=g++
CPPFLAGS=-O3 -std=c++17 

HPP=hipcc
HPPFLAGS=-O3 
INCDIR=/opt/rocm/include/hip

all: xtstf16

xtstf16: tstf16.cpp
	$(HPP) $(HPPFLAGS) -I$(INCDIR) -o xtstf16 tstf16.cpp 
clean: 
	- rm xtstf16 
