// C++ standard libraries.
#include <iostream>
using namespace std;

// Linux file i/o and terminal i/o.
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

// OpenGL libraries.
#include <GL/glew.h>
#include <SDL2/SDL.h>

// Discrete Fourier Transfrom library.
#include "test_dft.hpp"

// Struct for holding the coordinates of a point.
struct point {
    GLfloat x;
    GLfloat y;
};

int serial_open()
{
    // Serial port name on Linux.
    // Options for opening the serial port.
    char serial_port[] {"/dev/ttyACM2"};
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
    // Read five bytes.
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

pair<GLuint, GLint> gui_init_resources(void)
{
    GLint compile_ok {GL_FALSE};
    GLint link_ok {GL_FALSE};

    // Compile GLSL vertex shader.
	GLuint vs {glCreateShader(GL_VERTEX_SHADER)};
	const char *vs_source {
		// "#version 100\n                         "  // OpenGL ES 2.0
		"#version 120\n                           "  // OpenGL 2.1
		"attribute vec2 coord2d;                  "
		"void main(void) {                        "
		"  gl_Position = vec4(coord2d, 0.0, 1.0); "
		"}                                        "
    };
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok) {
		cerr << "Failed, could not compile vertex shader." << endl;
		return {make_pair(-1, -1)};
	}

    // Compile GLSL fragment shader.
    GLuint fs {glCreateShader(GL_FRAGMENT_SHADER)};
	const char *fs_source {
		// "#version 100\n                               "  // OpenGL ES 2.0
		"#version 120\n                               "  // OpenGL 2.1
		"void main(void) {                            "
		"  gl_FragColor[0] = gl_FragCoord.x/640.0;    "
		"  gl_FragColor[1] = gl_FragCoord.y/480.0;    "
		"  gl_FragColor[2] = 0.5;                     "
		"}                                            "
    };
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok) {
		cerr << "Failed, could not compile fragment shader." << endl;
		return {make_pair(-1, -1)};
    }

    // Link GLSL objectives.
    GLuint gui_program {glCreateProgram()};
	glAttachShader(gui_program, vs);
	glAttachShader(gui_program, fs);
	glLinkProgram(gui_program);
	glGetProgramiv(gui_program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "Failed, could not run glLinkProgram." << endl;
		return {make_pair(-1, -1)};
	}

    // Bind attribute to the GLSL program.
    const char* gui_attribute_name {"coord2d"};
	GLint gui_attribute {glGetAttribLocation(gui_program, gui_attribute_name)};
	if (gui_attribute == -1) {
		cerr << "Failed, could not bind attribute " << gui_attribute_name << "." << endl;
		return {make_pair(-1, -1)};
	}

    pair<GLuint, GLint> p {make_pair(gui_program, gui_attribute)};
    return p;
}

void gui_render(SDL_Window* window, GLuint gui_program, GLint gui_attribute, point (*graph)[512])
{
    // Clear the background to white.
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

    // Set the program and attribute to be used.
	glUseProgram(gui_program);
	glEnableVertexAttribArray(gui_attribute);

    GLuint vbo;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(*graph), *graph, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(gui_attribute);
    glVertexAttribPointer(
        gui_attribute,  // attribute
        2,              // number of elements per vertex, here (x,y)
        GL_FLOAT,       // the type of each element
        GL_FALSE,       // take our values as-is
        0,              // no space between values
        0               // use the vertex buffer object
    );

    glDrawArrays(GL_LINE_STRIP, 0, 512);

    glDisableVertexAttribArray(gui_attribute);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Display window.
	SDL_GL_SwapWindow(window);
}

void gui_free_resources(GLuint gui_program)
{
    glDeleteProgram(gui_program);
}

void gui_mainloop(SDL_Window* window, GLuint gui_program, GLint gui_attribute)
{
	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				return;
		}
		// gui_render(window, gui_program, gui_attribute);
	}
}

int main()
{
    // Initialise SDL.
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window {SDL_CreateWindow("Testbed", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL)};
	SDL_GL_CreateContext(window);

	// Initialise extension wrangler.
	GLenum glew_status {glewInit()};
	if (glew_status != GLEW_OK) {
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}

    // Initialise GUI resources.
    pair<GLuint, GLint> result = gui_init_resources();
    GLuint gui_program {result.first};
    GLint gui_attribute {result.second};
	if (gui_program == -1 || gui_attribute == -1) {
        return EXIT_FAILURE;
    }

    // Open the Arduino serial port.
    int fd = serial_open();

    int reading {};
    static SlidingDFT<double, 512> dft;
    complex<double> DC_bin;

    point graph[512];

    while (!dft.is_data_valid()) {
        reading = serial_read(fd);
        dft.update(double(reading));
    }

	// Display the window.
    while (true) {
        reading = serial_read(fd);
        dft.update(double(reading));
        for(int i = 0; i < 512; i++) {
            DC_bin = dft.dft[i];
            graph[i].x = i * 10.0 / 511.0;
            graph[i].y = abs(DC_bin) / 20000.0;
        }
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                goto end;
        }
        gui_render(window, gui_program, gui_attribute, &graph);
    }
end:

    // Free GUI resoureces.
	gui_free_resources(gui_program);

    // Close the Arduino serial port.
    if (serial_close(fd) == -1) {
        cout << "Failed, program terminates." << endl;
        return -1;
    }

	return EXIT_SUCCESS;
}
