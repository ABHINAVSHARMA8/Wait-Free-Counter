all:	counter.cpp
	g++ -std=c++11  -pthread counter.cpp
	

clean:	
	rm a.out