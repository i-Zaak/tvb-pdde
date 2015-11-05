all:bin/pdde bin/tests

bin/pdde:src/pdde.cc src/history.cc src/history.h
	g++ -g src/pdde.cc src/history.cc -o bin/pdde

bin/tests:tests/history_tests.cc src/history.cc src/history.h
	g++ -Isrc tests/history_tests.cc src/history.cc -o bin/tests

clean:
	rm -rf bin/*
