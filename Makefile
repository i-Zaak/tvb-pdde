all:bin/tests bin/mpi_tests bin/seq_bench bin/mpi_bench

# use following variables to control the build:
# OPENMP=[1|0]		adds OpenMP compiler flags, default 0 
# INSTRUMENT=[1|0]	uses scalasca to instrument MPI parts, default 0
# DEBUG=[1|0]		turns of -O optimization and adds -g flag, default 0
# PROFILE=[1|0]		turns of -O optimization and adds -g and -pg flag, default 0

ifeq (INSTRUMENT,1)
	MPI_CC = scalasca -instrument mpic++
else
	MPI_CC = mpic++
endif
CC = g++
bin/mpi_bench:CC = $(MPI_CC)
bin/mpi_tests:CC = $(MPI_CC)
CFLAGS = -Wall 

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0
else ifeq ($(PROFILE),1)
	CFLAGS += -g -O0
else
	CFLAGS += -O3
endif

ifeq ($(PROFILE),1)
	CFLAGS += -pg 
endif

ifeq ($(OPENMP),1)
	CFLAGS += -fopenmp
endif

INCLUDES = -Isrc
LFLAGS =
LIBS =
SUFFIXES += .d


TEST_SOURCES = tests/main.cc \
			   tests/history_tests.cc \
			   tests/coupling_tests.cc \
			   tests/model_tests.cc \
			   tests/integrator_tests.cc \
			   tests/observer_tests.cc \
			   tests/connectivity_tests.cc \
			   tests/common_tests.cc

TEST_OBJS = $(TEST_SOURCES:.cc=.o)

MPI_TEST_SOURCES = tests/mpi_main.cc
MPI_TEST_OBJS = $(MPI_TEST_SOURCES:.cc=.o)

SOURCES = src/coupling.cc \
		  src/history.cc \
		  src/model.cc \
		  src/integrator.cc \
		  src/observer.cc \
		  src/connectivity.cc 
OBJS = $(SOURCES:.cc=.o)

MPI_SOURCES = src/mpi_integrator.cc
MPI_OBJS = $(MPI_SOURCES:.cc=.o)

TOOL_SOURCES = src/seq_bench.cc 
TOOL_OBJS = $(TOOL_SOURCES:.cc=.o)

MPI_TOOL_SOURCES = src/mpi_bench.cc 
MPI_TOOL_OBJS = $(MPI_TOOL_SOURCES:.cc=.o)


SRC= $(SOURCES) $(MPI_SOURCES) $(MPI_TEST_SOURCES) $(TEST_SOURCES) $(TOOL_SOURCES) $(TOOL_SOURCES)
DEPFILES:=$(patsubst %.cc,%.d,$(SRC))
%.d: %.cc
	$(CC) $(CFLAGS) $(INCLUDES) -MM -MT '$(patsubst %.cc,%.o,$<)' $< -MF $@

-include $(DEPFILES)

bin/mpi_bench: $(MPI_TOOL_OBJS) $(OBJS) $(MPI_OBJS) bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/mpi_bench $(MPI_TOOL_OBJS) $(MPI_OBJS) $(OBJS) $(LFLAGS) $(LIBS)

bin/seq_bench: $(TOOL_OBJS) $(OBJS) bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/seq_bench $(TOOL_OBJS) $(OBJS) $(LFLAGS) $(LIBS)

bin/tests: $(TEST_OBJS) $(OBJS) bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/tests $(TEST_OBJS) $(OBJS) $(LFLAGS) $(LIBS)

bin/mpi_tests: $(MPI_TEST_OBJS) $(OBJS) $(MPI_OBJS) bin
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/mpi_tests $(MPI_TEST_OBJS) $(MPI_OBJS) $(OBJS) $(LFLAGS) $(LIBS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

bin:
	mkdir $@

clean:
	rm -rf bin/* src/*.o tests/*.o src/*.d tests/*.d

test:bin/tests bin/mpi_tests
	@echo "Sequential tests"
	./bin/tests
	@echo "MPI tests"
	mpirun -n 2 ./bin/mpi_tests

test-gdb:bin/tests
	gdb ./bin/tests
