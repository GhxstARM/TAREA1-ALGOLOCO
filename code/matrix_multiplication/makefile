CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
LDFLAGS ?= -lpsapi

TARGET := matrix_multiplication.exe
SOURCES := matrix_multiplication.cpp algorithms/naive.cpp algorithms/strassen.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES) algorithms/matrix_algorithms.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

# Example: make run LEFT=data/matrix_input/16_densa_D0_a_1.txt RIGHT=data/matrix_input/16_densa_D0_a_2.txt ALGORITHM=all
run: $(TARGET)
	./$(TARGET) $(LEFT) $(RIGHT) $(if $(ALGORITHM),$(ALGORITHM),all)

clean:
	$(RM) $(TARGET)
