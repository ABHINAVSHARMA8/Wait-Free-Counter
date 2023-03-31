all:	counter.cpp
	g++ -std=c++11  -pthread counter.cpp
	./a.out

clean:	
	rm a.out