all:bin/pdde bin/tests 

bin/pdde:src/pdde.cc src/history.cc src/history.h bin
	g++ -Wall -g src/pdde.cc src/history.cc -o bin/pdde
bin:
	mkdir $@

bin/tests:tests/history_tests.cc tests/coupling_tests.cc src/coupling.cc src/history.cc src/history.h
	g++ -Wall -g -Isrc tests/history_tests.cc tests/coupling_tests.cc src/history.cc src/coupling.cc -o bin/tests

clean:
	rm -rf bin/*

runtests:bin/tests
	./bin/tests
