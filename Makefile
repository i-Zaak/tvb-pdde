
# use following variables to control the build:
# OPENMP=[1|0]		adds OpenMP compiler flags, default 0 
# MPI=[1|0]			enables MPI, default 0 
# INSTRUMENT=[1|0]	uses scalasca to instrument MPI parts, default 0, also forces MPI and PROFILE
# DEBUG=[1|0]		turns of -O optimization and adds -g flag, default 0
# PROFILE=[1|0]		turns of -O optimization and adds -g and -pg flag, default 0

OPENMP=0
MPI=0
INSTRUMENT=0
DEBUG=0
PROFILE=0

ifeq ($(INSTRUMENT),1)
	MPICC = scalasca -instrument mpic++
	MPI=1
	PROFILE=1
else
	MPICC = gccfilter -c mpic++
endif

ifeq ($(MPI),1)
	CC = $(MPICC)
all:bin/tests bin/mpi_tests bin/seq_bench bin/mpi_bench
else
	CC = gccfilter -c g++
all:bin/tests bin/seq_bench 
endif

CFLAGS = -Wall 

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0
else ifeq ($(PROFILE),1)
	CFLAGS += -g -O0
else
	CFLAGS += -Ofast
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
			   tests/random_tests.cc \
			   tests/common_tests.cc

TEST_OBJS = $(TEST_SOURCES:.cc=.o)

MPI_TEST_SOURCES = tests/mpi_main.cc \
				   tests/mpi_history_tests.cc
MPI_TEST_OBJS = $(MPI_TEST_SOURCES:.cc=.o)

SOURCES = src/coupling.cc \
		  src/history.cc \
		  src/model.cc \
		  src/integrator.cc \
		  src/observer.cc \
		  src/connectivity.cc \
		  src/random.cc
OBJS = $(SOURCES:.cc=.o)

MPI_SOURCES = src/mpi_history.cc
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

ifeq ($(MPI),1)
test:bin/tests bin/mpi_tests
	@echo "Sequential tests"
	./bin/tests
	@echo "MPI tests"
	mpirun -n 4 ./bin/mpi_tests
else
test:bin/tests
	@echo "Sequential tests"
	./bin/tests
endif

test-gdb:bin/tests
	gdb ./bin/tests
