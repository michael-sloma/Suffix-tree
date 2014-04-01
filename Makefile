CC=g++
CFLAGS=-c -g -Wall -I $(BOOST_LOCATION) -std=c++0x -D UKKONEN
LDFLAGS=
SOURCES= main.cpp suffix_tree.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=substring

all: $(SOURCES) $(EXECUTABLE)
	
clean:
	rm -rf *.o

realclean:
	rm -rf *.o $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

main.o: suffix_tree.h
suffix_tree.o: suffix_tree.h 
