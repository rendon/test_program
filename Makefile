CXX=g++
tp: tp.cpp
	$(CXX) -Wall -o tp tp.cpp

install: tp
	sudo cp -v tp /usr/bin/tp

