CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

TARGET = doc_version_system

all: $(TARGET)

$(TARGET): main.cpp structures.h manager_ll.h manager_stack.h benchmark.h
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

benchmark:
	./$(TARGET) --benchmark

clean:
	rm -f $(TARGET) data_ll.txt data_stack.txt data_bst_ll.txt data_bst_stack.txt benchmark_results.csv benchmark_memory.csv

.PHONY: all benchmark clean
