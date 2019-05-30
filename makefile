# makefile

CXX = clang++
CXXFLAGS = -O3

OPENGL_LIBS = -lglut -lGLEW -lGL -lSDL2

linux_io.o:
	$(CXX) $(CXXFLAGS) -o linux_io.o linux_io.cpp

# test_opengl.o:
# 	$(CXX) $(CXXFLAGS) $(OPENGL_LIBS) -o test_opengl.o test_opengl.cpp

# test_opencv.o:
# 	$(CXX) $(CXXFLAGS) -o test_opencv.o test_opencv.cpp

clean:
	rm *.o
