all: dec.cpp dec.h main.cpp
	g++ -g dec.cpp main.cpp -o dec
clean:
	rm -rf dec
