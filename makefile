# makefile

CXX = clang++
CXXFLAGS = -O3

OPENGL_LIBS = -lglut -lGLEW -lGL -lSDL2

# serial.o:
# 	$(CXX) $(CXXFLAGS) -o serial.o serial.cpp

test_opengl.o:
	$(CXX) $(CXXFLAGS) $(OPENGL_LIBS) -o test_opengl.o test_opengl.cpp

clean:
	rm *.o
