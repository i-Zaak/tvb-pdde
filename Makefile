all:bin/pdde

bin/pdde:src/pdde.cc src/history.cc
	g++ -g src/pdde.cc src/history.cc -o bin/pdde
