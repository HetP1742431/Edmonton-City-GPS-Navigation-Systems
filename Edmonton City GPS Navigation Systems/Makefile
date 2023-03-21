# Name: Het Bharatkumar Patel
# SID: 1742431
# CCID: hetbhara
# AnonID: 1000348298
# CMPUT 275, Winter 2023

# Assignment #1 Part 1: Navigation System
# -------------------------------------------

server: server.o dijkstra.o digraph.o
	g++ server.o dijkstra.o digraph.o -o server

server.o: server.cpp
	g++ -c server.cpp

dijkstra.o: dijkstra.cpp dijkstra.h
	g++ -c dijkstra.cpp

digraph.o: digraph.cpp digraph.h wdigraph.h
	g++ -c digraph.cpp

clean:
	rm *.o server