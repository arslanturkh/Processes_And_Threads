test: Homework1.o 
	g++ Homework1.o -o test -pthread -std=c++11
	
Homework1.o: hw1.cpp
	g++ -c Homework1.cpp -pthread -std=c++11 
	
clean: 
	rm *.o test
