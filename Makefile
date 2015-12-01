all:bin/tests 

CC = g++
CFLAGS = -Wall -g
INCLUDES = -Isrc
LFLAGS =
LIBS =
SUFFIXES += .d


TEST_SOURCES = tests/main.cc \
			   tests/history_tests.cc \
			   tests/coupling_tests.cc \
			   tests/model_tests.cc \
			   tests/integrator_tests.cc \
			   tests/observer_tests.cc

TEST_OBJS = $(TEST_SOURCES:.cc=.o)

SOURCES = src/coupling.cc \
		  src/history.cc \
		  src/model.cc \
		  src/integrator.cc \
		  src/observer.cc 

OBJS = $(SOURCES:.cc=.o)

SRC= $(SOURCES) $(TEST_SOURCES)
DEPFILES:=$(patsubst %.cc,%.d,$(SRC))
%.d: %.cc
	$(CC) $(CFLAGS) $(INCLUDES) -MM -MT '$(patsubst %.cc,%.o,$<)' $< -MF $@

-include $(DEPFILES)

bin/tests: $(TEST_OBJS) $(OBJS) bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/tests $(TEST_OBJS) $(OBJS) $(LFLAGS) $(LIBS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

bin:
	mkdir $@

clean:
	rm -rf bin/* src/*.o tests/*.o src/*.d tests/*.d

test:bin/tests
	./bin/tests

test-gdb:bin/tests
	gdb ./bin/tests
