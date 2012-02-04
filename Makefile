All : main.o filter.o
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -o LaserDo main.o filter.o
main.o :main.cpp mycv.h filter.h
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -o main.o -c main.cpp
filter.o: filter.cpp
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -o filter.o -c filter.cpp

