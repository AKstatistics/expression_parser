SHELL=/bin/sh
.SUFFIXES:
.SUFFIXES: .cpp .o
CC=g++
CFLAGS=-c -Wall -std=c++11

SOURCES=main.cpp parse.cpp parse_num.cpp parse_bin.cpp parse_hex.cpp clean_input.cpp
OBJECTS=$(SOURCES:.cpp=.o)

calc_test_build: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

calc : $(OBJECTS)
	$(CC) $(OBJECTS) -o calc

main.o : main.cpp parser.h clean_input.cpp
	$(CC) $(CFLAGS) main.cpp

parse.o : parse.cpp parser.h common_parse_includes.h
	$(CC) $(CFLAGS) parse.cpp

parse_num.o : parse_num.cpp common_parse_includes.h
	$(CC) $(CFLAGS) parse_num.cpp

parse_bin.o : parse_bin.cpp common_parse_includes.h
	$(CC) $(CFLAGS) parse_bin.cpp

parse_hex.o : parse_hex.cpp common_parse_includes.h
	$(CC) $(CFLAGS) parse_hex.cpp

clean_input.o : clean_input.cpp common_parse_includes.h
	$(CC) $(CFLAGS) clean_input.cpp

.PHONY : clean

clean :
	rm ./*.o ./calc_test_build
