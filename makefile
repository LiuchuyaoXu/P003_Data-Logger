# makefile

CXX = clang++
CXXFLAGS = -O3

OPENGL_LIBS = -lglut -lGLEW -lGL -lSDL2

# main.o:
# 	$(CXX) $(CXXFLAGS) $(OPENGL_LIBS) -o main.o main.cpp

test_main.o:
	$(CXX) $(CXXFLAGS) $(OPENGL_LIBS) -o test_main.o test_main.cpp

# test_serial.o:
# 	$(CXX) $(CXXFLAGS) -o test_serial.o test_serial.cpp

# test_opengl.o:
# 	$(CXX) $(CXXFLAGS) $(OPENGL_LIBS) -o test_opengl.o test_opengl.cpp

clean:
	rm *.o
