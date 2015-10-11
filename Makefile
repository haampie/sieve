CXX = g++
CXXFLAGS = -Wall -std=c++11 -MMD -O3
CXXINCL = -I../bsp/include
LINKER = $(CXX)
LDFLAGS =
LDLIBS = -L../bsp/lib -lmcbsp1.2.0 -lpthread

TARGET = bin/main
SOURCES = main.cc $(wildcard src/*.cc)
OBJECTS = $(SOURCES:.cc=.o)

DEPS = $(OBJECTS:.o=.d)
ODIR = ./objects

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINKER) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $(CXXINCL) $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)

-include $(DEPS)
