# makefile

CXX = clang++
CXXFLAGS = -O3

linux_io.o:
	$(CXX) $(CXXFLAGS) -lglut -lGLU -lGL -o linux_io.o linux_io.cpp

clean:
	rm *.o
