CXX = g++
CXXFLAGS = -O2 -std=c++17

OBJ = main.o tests.o longArithmetic.o

default: executable

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

executable: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o executable

test: executable
	./executable test

pi: executable
	./executable pi

clean:
	rm -f $(OBJ) executable