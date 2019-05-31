#include <cstdlib>
#include <iostream>
using namespace std;

// Use glew.h instead of gl.h to get all the GL prototypes declared.
// Use SDL2 for the base window and OpenGL context initialisation.
#include <GL/glew.h>
#include <SDL2/SDL.h>

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
		// "#version 100\n           "  // OpenGL ES 2.0
		"#version 120\n           "  // OpenGL 2.1
		"void main(void) {        "
		"  gl_FragColor[0] = 0.0; "
		"  gl_FragColor[1] = 0.0; "
		"  gl_FragColor[2] = 1.0; "
		"}                        "
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

    // Bind name to the GLSL program.
    const char* gui_attribute_name {"coord2d"};
	GLint gui_attribute {glGetAttribLocation(gui_program, gui_attribute_name)};
	if (gui_attribute == -1) {
		cerr << "Failed, could not bind attribute " << gui_attribute_name << "." << endl;
		return {make_pair(-1, -1)};
	}

    pair<GLuint, GLint> p {make_pair(gui_program, gui_attribute)};
    return p;
}

void gui_render(SDL_Window* window, GLuint gui_program, GLint gui_attribute)
{
    // Clear the background to white.
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(gui_program);
	glEnableVertexAttribArray(gui_attribute);
	GLfloat triangle_vertices[] {
	    0.0,  0.8,
	   -0.8, -0.8,
	    0.8, -0.8,
	};
	// Describe vertices array to OpenGL
	glVertexAttribPointer(
		gui_attribute,    // Attribute.
		2,                // Number of elements per vertex, here (x,y).
		GL_FLOAT,         // Type of each element.
		GL_FALSE,         // Take our values as-is.
		0,                // No extra data between each position.
		triangle_vertices // Pointer to the C array.
	);

	// Push elemtns in buffer_vertices to the vertex shader.
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(gui_attribute);

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
		gui_render(window, gui_program, gui_attribute);
	}
}

int main(int argc, char* argv[])
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

	// Display the window.
	gui_mainloop(window, gui_program, gui_attribute);

    // Free GUI resoureces.
	gui_free_resources(gui_program);
	return EXIT_SUCCESS;
}
