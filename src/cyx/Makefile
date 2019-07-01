CC = g++
GRBPATH = /opt/gurobi811/

main: main.o Show.o Graph.o
	$(CC) main.o Show.o Graph.o -o main -L$(GRBPATH)/linux64/lib -lgurobi_c++ -lgurobi81 

main.o: main.cc
	$(CC) -c main.cc -o main.o -I$(GRBPATH)/linux64/include -Iinclude/

Show.o: Show.cc
	$(CC) -c Show.cc -o Show.o -Iinclude/


Graph.o: Graph.cc
	$(CC) -c Graph.cc -o Graph.o -Iinclude/  -I$(GRBPATH)/linux64/include

