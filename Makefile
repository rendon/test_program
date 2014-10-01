CXX=g++
tp: tp.cpp
	$(CXX) -std=c++11 -Wall -o tp tp.cpp

install: tp
	cp -v tp /usr/bin/tp
	cp -v cf-parser /usr/bin/cf-parser
	cp -v tp-runner /usr/bin/tp-runner
	cp -v tp-cleaner /usr/bin/tp-cleaner
uninstall:
	rm -v /usr/bin/tp
	rm -v /usr/bin/cf-parser
	rm -v /usr/bin/tp-runner
	rm -v /usr/bin/tp-cleaner

