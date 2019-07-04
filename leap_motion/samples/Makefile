SHELL = /bin/sh
CC = g++
CFLAGS = -Wall -std=c++14 -O3 -g
CPPFLAGS = -I. -I../include
LDFLAGS =../lib/libLeap.dylib # -pthread 
SRC = sample.cpp
OBJ = $(SRC:.cpp=.o)

CPPFLAGS += -I/usr/local/include
LDFLAGS += -L/usr/local/lib
CPPFLAGS += $(shell sdl2-config --cflags)
LDFLAGS  += -lm -framework OpenGL -lGL4Dummies $(shell sdl2-config --libs)

%.o:	%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

sample: 	$(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib ../lib/libLeap.dylib sample
endif

clean:
	rm -rf *.o sample

# SHELL = /bin/sh
# OS = $(shell uname)
# CC = g++
# CFLAGS=-Wall -std=c++14 -O3
# LDFLAGS = ../lib/libLeap.dylib -lGL4Dummies $(shell sdl2-config --libs)
# 
# sample: sample.cpp
# 	$(CC) -Wall -g -I../include sample.cpp -o sample $(LEAP_LIBRARY)
# ifeq ($(OS), Darwin)
# 	install_name_tool -change @loader_path/libLeap.dylib ../lib/libLeap.dylib sample
# endif
# 
# clean:
# 	rm -rf sample *.o