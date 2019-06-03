// C++ standard libraries.
#include <cstdlib>
#include <fstream>
#include <string>
#include <tuple>
#include <iostream>
using namespace std;

// Linux file i/o and terminal i/o.
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

// Opengl libraries.
#include <GL/glew.h>
#include <SDL2/SDL.h>

// Discrete Fourier Transfrom library.
#include "lib_dft.hpp"

int serial_open()
{
    // Serial port name on Linux.
    // Options for opening the serial port.
    char serial_port[] {"/dev/ttyACM0"};
    int serial_port_options {O_RDWR | O_NOCTTY};

    // Attributes of the serial port.
    long serial_port_cflags {B9600 | CRTSCTS | CS8 | CLOCAL | CREAD};
    long serial_port_iflags {IGNPAR | ICRNL};
    long serial_port_oflags {0};
    long serial_port_lflags {ICANON};

    // Open the serial port.
    int fd {open(serial_port, serial_port_options)};
    if (fd == -1) {
        cout << "Failed, could not open the open the serial port." << endl;
        return -1;
    }

    // Configure the serial port attrubutes.
    termios serial_port_attributes {};
    tcgetattr(fd, &serial_port_attributes);
    serial_port_attributes.c_cflag = serial_port_cflags;
    serial_port_attributes.c_iflag = serial_port_iflags;
    serial_port_attributes.c_oflag = serial_port_oflags;
    serial_port_attributes.c_lflag = serial_port_lflags;
    if (tcsetattr(fd, TCSANOW, &serial_port_attributes) == -1) {
        cout << "Failed, could not set the serial port attributes." << endl;
        return -1;
    }

    return fd;
}

int serial_read(int fd)
{
    // Read five bytes or one line if in canonical mode.
    char buff[5] {};
    int bytes_to_read {5};
    long bytes_read {read(fd, buff, bytes_to_read)};
    if (bytes_read == -1) {
        cout << "Failed, could not read from the serial port." << endl;
    }

    // Convert the reading into integer.
    int result {};
    if (bytes_read == 5) {
        result = (buff[0] - '0') * 1000 + (buff[1] - '0') * 100 + (buff[2] - '0') * 10 + (buff[3] - '0');
    }
    else if (bytes_read == 4) {
        result = (buff[0] - '0') * 100 + (buff[1] - '0') * 10 + (buff[2] - '0');
    }
    else if (bytes_read == 3) {
        result = (buff[0] - '0') * 10 + (buff[1] - '0');
    }
    else{
        result = (buff[0] - '0');
    }
    return result;
}

int serial_close(int fd)
{
    if (close(fd) == -1) {
        cout << "Failed, could not close the serial port." << endl;
        return -1;
    }
    return 1;
}

string file_read(string filename)
{
	// Open the file specified by 'filename'.
	fstream fs {filename, ios_base::in};
	if (!fs.is_open()) {
		cerr << "Failed, could not open " << filename << "." << endl;
		return "";
	}

	// Read from the file.
	string result {};
	char x {};
	while ((x = fs.get()) != EOF) {
			result += x;
	}

	// Close the file.
	// Return the result.
	fs.close();
	return result;
}

void opengl_shader_log(GLuint object)
{
	GLint log_length = 0;

	// Check the type of the object.
	if (glIsShader(object)) {
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	}
	else if (glIsProgram(object)) {
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	}
	else {
		cerr << "Failed, object is either a shader nor a program." << endl;
		return;
	}

	char* log = (char*)malloc(log_length);

	// Print the log of the object.
	if (glIsShader(object)) {
		glGetShaderInfoLog(object, log_length, NULL, log);
	}
	else if (glIsProgram(object)) {
		glGetProgramInfoLog(object, log_length, NULL, log);
	}
	cerr << log << endl;

	free(log);
}

GLuint opengl_create_shader(string filename, GLenum shader_type)
{
	// Read the GLSL code from the file.
	// For description of the GLSL code, read
	// 	https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Introduction
	string shader_code {file_read(filename)};
	if (shader_code == "") {
		cerr << "Failed, could not read from " << filename << endl;
		return -1;
	}
	const GLchar* shader_source {shader_code.c_str()};

	// Generate a shader.
    // Pass the source code string to the shader.
	GLuint shader {glCreateShader(shader_type)};
	glShaderSource(shader, 1, &shader_source, NULL);

	// Compile the shader.
	// Display error log.
	glCompileShader(shader);
	GLint compile_ok {GL_FALSE};
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		cerr << "Failed, could not compile " << filename << ":" << endl;
		opengl_shader_log(shader);
		glDeleteShader(shader);
		return -1;
	}

	return shader;
}

tuple<GLuint, GLint, GLuint> opengl_init_shader()
{
	// Create and compile the vertex shader and the fragment shader.
	GLuint vs {};
	GLuint fs {};
	string vs_source_file {"lib_opengl_vs.glsl"};
	string fs_source_file {"lib_opengl_fs.glsl"};
	if ((vs = opengl_create_shader(vs_source_file, GL_VERTEX_SHADER))   == -1) {
		cerr << "Failed, could not create shader from " << vs_source_file << "." << endl;
		return {make_tuple(-1, -1, -1)};
	}
	if ((fs = opengl_create_shader(fs_source_file, GL_FRAGMENT_SHADER)) == -1) {
		cerr << "Failed, could not create shader from " << fs_source_file << "." << endl;
		return {make_tuple(-1, -1, -1)};
	}

    // Link the vertex shader and the fragment shader to form the overall program.
	GLint link_ok {GL_FALSE};
    GLuint shader_program {glCreateProgram()};
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "Failed, could not link the shader programs: " << endl;
		opengl_shader_log(shader_program);
		return {make_tuple(-1, -1, -1)};
	}

    // Bind the C++ variable 'shader_attribute' to the attribute in the GLSL program
	//	specified by 'shader_attribute_name'.
    const char shader_attribute_name[] {"coord2d"};
    GLint shader_attribute {glGetAttribLocation(shader_program, shader_attribute_name)};
    if (shader_attribute == -1) {
        cerr << "Failed, could not bind the shader program attribute." << endl;
        return {make_tuple(-1, -1, -1)};
    }

	// Initialise the vertex buffer object.
	// Generate 1 vbo and make it the current active buffer.
	// Bind the vbo to the array buffer in the graphic card.
	GLuint vbo {};
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    tuple<GLuint, GLint, GLuint> shader_descriptors {make_tuple(shader_program, shader_attribute, vbo)};
    return shader_descriptors;
}

void opengl_free_shader(GLuint shader_program, GLuint vbo)
{
    glDeleteProgram(shader_program);
	glDeleteBuffers(1, &vbo);
}

void opengl_render(SDL_Window* window, GLuint shader_program, GLint shader_attribute, GLuint vbo, int fd)
{
	// // Enable alpha channel for transparency.
	// // The transparency is determined by the 'gl_FragColor[3]' in the fragment shader GLSL code.
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clear the background to white.
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

    // Bind the shader program and attribute.
	glUseProgram(shader_program);
	glEnableVertexAttribArray(shader_attribute);

	// Take a reading from the serial port.
	// Update the discrete Fourier transform.
	// Copy values into the vertices array.
	struct point {
	    GLfloat x;
	    GLfloat y;
	};
	static SlidingDFT<double, 512> dft;
	complex<double> dft_value;
	point graph[512];
	int reading {};
	do {
		reading = serial_read(fd);
		dft.update(double(reading));
	} while (!dft.is_data_valid());
	for (int i {}; i < 512; i++) {
		dft_value = dft.dft[i];
		graph[i].x = (i - 256.0) / 256.0;
		graph[i].y = abs(dft_value) / 20000.0;
	}

	// Push the vertices to the array buffer in the graphic card, which
	//	is bound to the vertex buffer object initialised in 'opengl_init_shader'.
	// GL_STATIC_DRAW indicates that the buffer will not be written to very often, and
	//	that the GPU should keep a copy of it in its own memory. It is always possible
	// 	to write new values to the vbo and thus the array buffer. If the data changes
	// 	once per frame or more often, use GL_DYNAMIC_DRAW or GL_STREAM_DRAW.
	glBufferData(GL_ARRAY_BUFFER, sizeof(graph), graph, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
  	glEnableVertexAttribArray(shader_attribute);
	// Describe vertices array to OpenGL.
	glVertexAttribPointer(
		shader_attribute, // Attribute.
		2,                // Number of elements per vertex, here (x,y).
		GL_FLOAT,         // Type of each element.
		GL_FALSE,         // Take our values as-is.
		0,                // No extra data between each position.
		0				  // Offset of the first element.
	);
	// Push elemtns in triangle_vertices to the vertex shader.
	glDrawArrays(GL_LINE_STRIP, 0, 512);

    // Display the window.
	glDisableVertexAttribArray(shader_attribute);
	SDL_GL_SwapWindow(window);
}

void opengl_mainloop(SDL_Window* window, GLuint shader_program, GLint shader_attribute, GLuint vbo, int fd)
{
	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				return;
		}
		opengl_render(window, shader_program, shader_attribute, vbo, fd);
	}
}

int main()
{
	// Open serial port.
	int fd {serial_open()};
	if (fd == -1) {
		cerr << "Failed, could not open the serial port." << endl;
		return EXIT_FAILURE;
	}

    // Initialise the window.
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window {SDL_CreateWindow("DFT Spectrum",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};
	SDL_GL_CreateContext(window);
	// Request an alpha channel for transparency.
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);

    // Initialise extension wrangler.
	GLenum glew_status {glewInit()};
	if (glew_status != GLEW_OK) {
		cerr << "Failed, glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}

    // Initialise shader program and vertex buffer object.
    tuple<GLuint, GLint, GLuint> shader_descriptors = opengl_init_shader();
    GLuint shader_program {get<0>(shader_descriptors)};
    GLint shader_attribute {get<1>(shader_descriptors)};
	GLuint vbo {get<2>(shader_descriptors)};
	if (shader_program == -1 || shader_attribute == -1 || vbo == -1) {
		cerr << "Failed, program terminates." << endl;
        return EXIT_FAILURE;
    }

    // Go into the main loop.
	opengl_mainloop(window, shader_program, shader_attribute, vbo, fd);

    // Free shader program.
	opengl_free_shader(shader_program, vbo);

	// Close serial port.
	serial_close(fd);

	return EXIT_SUCCESS;
}
