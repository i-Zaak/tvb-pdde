all:bin/pdde bin/tests

bin/pdde:src/pdde.cc src/history.cc src/history.h bin
	g++ -g src/pdde.cc src/history.cc -o bin/pdde
bin:
	mkdir $@

bin/tests:tests/history_tests.cc src/history.cc src/history.h
	g++ -Isrc tests/history_tests.cc src/history.cc -o bin/tests

clean:
	rm -rf bin/*
