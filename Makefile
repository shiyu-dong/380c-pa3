all: cfg.cpp dce.h main.cpp
	g++ -g cfg.cpp main.cpp -o dce
clean:
	rm -rf dce dce.dSYM/ 
