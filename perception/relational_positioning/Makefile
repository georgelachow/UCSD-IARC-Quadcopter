CC=g++ -std=c++11
CFLAGS=-Wall -g
LIBS=`pkg-config opencv --cflags --libs`
OUTPUT=rel_pos

main: main.o roomba.o tracker.o
	$(CC) $(CFLAGS) -o $(OUTPUT) main.o $(LIBS)

main.o: main.cpp settings.hpp
	$(CC) $(CFLAGS) -c main.cpp $(LIBS)

roomba.o: roomba.hpp roomba.cpp
	$(CC) $(CFLAGS) -c roomba.cpp $(LIBS)

tracker.o: tracker.hpp roomba.hpp helpers.hpp
	$(CC) $(CFLAGS) -c tracker.cpp $(LIBS)

helpers.o: helpers.hpp helpers.cpp
	$(CC) $(CFLAGS) -c helpers.cpp $(LIBS)

clean:
	rm -rf *.o
