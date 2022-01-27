#Makefile
LDLIBS += -lpcap

airodump: main.o dot11.o
	gcc -o airodump main.o dot11.o -lpcap

main.o: main.cpp
	g++ -c -o main.o main.cpp

dot11.o: dot11.cpp
	g++ -c -o dot11.o dot11.cpp

clean:
	rm -f airodump *.o