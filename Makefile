all: cfg.cpp dce.cpp dce.h main.cpp
	g++ -g main.cpp cfg.cpp dce.cpp -o dce
clean:
	rm -rf dce dce.dSYM/ 
