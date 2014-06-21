CXX=g++
tp: tp.cpp
	$(CXX) -std=c++11 -Wall -o tp tp.cpp

install: tp
	cp -v tp /usr/bin/tp
	cp -v cf_parser /usr/bin/cf_parser
uninstall:
	rm -v /usr/bin/tp
	rm -v /usr/bin/cf_parser

