All : main.o filter.o FSM.o Xoutput.o GR.o
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -lX11 -o LaserDo main.o filter.o FSM.o Xoutput.o GR.o
main.o :main.cpp mycv.h filter.h FSM.h
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -o main.o -c main.cpp
filter.o: filter.cpp
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -o filter.o -c filter.cpp
FSM.o: FSM.cpp
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -o FSM.o -c FSM.cpp
Xoutput.o: Xoutput.cpp
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -lX11 -o Xoutput.o -c Xoutput.cpp
GR.o: GR.cpp
	g++ -g -pg -Wall -I/usr/local/include/opencv -L/usr/local/lib -lcv -lm -lhighgui -lcvaux -lX11 -o GR.o -c GR.cpp

