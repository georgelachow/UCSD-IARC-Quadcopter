CC=g++ -std=c++11
CFLAGS=-Wall -g
LIBS=`pkg-config opencv --cflags --libs`
OUTPUT=rel_pos

main: main.o roomba.o tracker.o helpers.o
	$(CC) $(CFLAGS) -o $(OUTPUT) main.o roomba.o tracker.o helpers.o $(LIBS)

main.o: main.cpp settings.hpp tracker.hpp
	$(CC) $(CFLAGS) -c main.cpp $(LIBS)

tracker.o: tracker.cpp tracker.hpp roomba.hpp helpers.hpp
	$(CC) $(CFLAGS) -c tracker.cpp $(LIBS)

roomba.o: roomba.cpp roomba.hpp
	$(CC) $(CFLAGS) -c roomba.cpp $(LIBS)

helpers.o: helpers.cpp helpers.hpp
	$(CC) $(CFLAGS) -c helpers.cpp $(LIBS)

clean:
	rm -rf *.o
