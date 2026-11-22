CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TESTS    = test_utils test_queue test_index test_graph_search test_sorting test_ranking

all: jobradius

jobradius: src/main.cpp src/*.h
	$(CXX) $(CXXFLAGS) -o jobradius src/main.cpp

test:
	mkdir -p build
	for t in $(TESTS); do $(CXX) $(CXXFLAGS) -o build/$$t tests/$$t.cpp && ./build/$$t || exit 1; done

clean:
	rm -rf build jobradius jobradius.exe
