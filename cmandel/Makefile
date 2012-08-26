
CXX ?= g++

LIB_SOURCES = mandel.cpp gdinterface.cpp
LIB_OBJECTS = $(LIB_SOURCES:.cpp=.o)
EXE_SOURCES = main.cpp
EXE_OBJECTS = $(EXE_SOURCES:.cpp=.o)
LIBRARY = libmandel.so
OPENMP ?= -fopenmp

GD_CFLAGS = $(shell gdlib-config --cflags)
GD_LIBS = $(shell gdlib-config --libs) -lgd

all: library standalone

library: $(LIB_OBJECTS)
	$(CXX) -shared $(LDFLAGS) $(GD_LIBS) $(OPENMP) $< -o $(LIBRARY)

standalone: $(LIB_OBJECTS) $(EXE_OBJECTS)
	$(CXX) $(LDFLAGS) $(GD_LIBS) $^ $(OPENMP) -o mandel

%.o: %.cpp
	$(CXX) $(CFLAGS) $(GD_CFLAGS) -fPIC -DPIC $(OPENMP) -c $< -o $@

clean:
	rm -f *.so *.o mandel

.PHONY: clean
