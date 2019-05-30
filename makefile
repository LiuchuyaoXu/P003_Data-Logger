# makefile

CXX = clang++
CXXFLAGS = -O3

linux_io.o:
	$(CXX) $(CXXFLAGS) -o linux_io.o linux_io.cpp

clean:
	rm *.o
