CXXINCL = -I./bsp/include
LDLIBS = -L./bsp/lib -lmcbsp1.2.0 -lpthread
CXX = g++
CXXFLAGS = -Wall -MMD -O3
LINKER = $(CXX)
LDFLAGS = -lrt

TARGET = bin/main
SOURCES = main.cc $(wildcard src/*.cc)
OBJECTS = $(SOURCES:.cc=.o)

TESTS_TARGET = bin/tests
TEST_SOURCES = tests/test_segment_boundaries_2.cc src/segment_boundaries_2.cc $(tests/*.cc)
TEST_OBJECTS = $(TEST_SOURCES:.cc=.o)

DEPS = $(OBJECTS:.o=.d)
ODIR = ./objects

all: $(TARGET)

tests: $(TESTS_TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(TESTS_TARGET): $(TEST_OBJECTS)
	$(LINKER) $(LDFLAGS) $(TEST_OBJECTS) $(LDLIBS) -o $@

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $(CXXINCL) $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) $(TESTS_TARGET) $(TEST_OBJECTS) $(DEPS)

-include $(DEPS)
