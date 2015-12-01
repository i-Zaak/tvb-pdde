all:bin/tests 

CC = g++
CFLAGS = -Wall -g
INCLUDES = -Isrc
LFLAGS =
LIBS =

TEST_SOURCES = tests/main.cc \
			   tests/history_tests.cc \
			   tests/coupling_tests.cc \
			   tests/model_tests.cc \
			   tests/integrator_tests.cc

TEST_OBJS = $(TEST_SOURCES:.cc=.o)

SOURCES = src/coupling.cc \
		  src/history.cc \
		  src/model.cc \
		  src/integrator.cc 

OBJS = $(SOURCES:.cc=.o)

bin/tests: $(TEST_OBJS) $(OBJS) bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/tests $(TEST_OBJS) $(OBJS) $(LFLAGS) $(LIBS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@


clean:
	    $(RM) $(OBJS)
bin:
	mkdir $@

clean:
	rm -rf bin/* src/*.o tests/*.o

test:bin/tests
	./bin/tests

test-gdb:bin/tests
	gdb ./bin/tests
