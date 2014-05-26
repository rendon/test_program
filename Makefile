CXX=g++
tp: tp.cpp
	$(CXX) -Wall -o tp tp.cpp

install: tp
	cp -v tp /usr/bin/tp
uninstall:
	rm -v /usr/bin/tp

