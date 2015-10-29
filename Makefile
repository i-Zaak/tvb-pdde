all:bin/pdde

bin/pdde:src/pdde.C 
	g++ src/pdde.C -o bin/pdde
