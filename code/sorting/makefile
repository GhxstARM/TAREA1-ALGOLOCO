CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
LDFLAGS ?= 

TARGET := sorting.exe
SOURCES := sorting.cpp algorithms/mergesort.cpp algorithms/quicksort.cpp algorithms/patiencesort.cpp algorithms/sort.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES) algorithms/sorting_algorithms.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

# Ejemplo de uso desde la terminal: 
# make run ALGO=quicksort N=1000 T=ascendente D=D1 M=a
run: $(TARGET)
	./$(TARGET) $(ALGO) $(N) $(T) $(D) $(M)

clean:
	$(RM) $(TARGET)
