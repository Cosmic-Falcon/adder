#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "global.h"
#include "adder.h"
#include "gfx.h"

bool keys[1024];

// Camera
GLfloat camera_x = 0;
GLfloat camera_y = 0;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode); // Register key presses
void key_parse(); // Act on key presses

int main() {
	adder::Polygon poly({{0, 0, 0}, {8, 32, 0}, {0, 64, 0}, {32, 56, 0}, {64, 64, 0}, {56, 32, 0}, {64, 0, 0}, {32, 8, 0}}, {256, 128, 0});
	poly.translate({0, 50, 0});
	std::cout << to_string(poly.vertices()) << std::endl;
	poly.rotate(3*PI/4, poly.get_position());


	std::cout << to_string(poly.vertices()) << std::endl;
	adder::Body body(100, 100, -.1, poly);
	gfx::init(3, 3, GL_FALSE);
	GLFWwindow* window = gfx::create_window(640, 480, "Adder Physics Engine");
	glfwSetKeyCallback(window, key_callback);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	GLuint vertex_shader = gfx::compile_shader("res/shaders/shader.vert", GL_VERTEX_SHADER);
	GLuint fragment_shader = gfx::compile_shader("res/shaders/shader.frag", GL_FRAGMENT_SHADER);
	GLuint shader_program = gfx::create_program({vertex_shader, fragment_shader});
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	
	// Create vertex array object
	// The vertex array object stores the states of the vertex and index buffer
	// objects defined inside of it, allowing them to be easily reused again.
	// The vertex and index buffer objects store the vertex and index data on
	// the gpu, respectively. They are cleared automatically after each drawing
	// cycle, hence the need for the vertex array object. I think.
	// TODO: find some way to abstract this
	GLuint vao, vbo, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, poly.get_gl_vertices_size(), poly.get_gl_vertices(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, poly.get_gl_indices_size(), poly.get_gl_indices(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	
	// Transformation matrices
	glm::mat4 model, view, projection;
	projection = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f);


	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		key_parse();

		glUseProgram(shader_program);

		// Update view matrix with new camera position
		view = glm::translate(glm::mat4(), glm::vec3(camera_x, camera_y, 0.0f));
		// Load uniforms
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(vao);	

		// Render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, poly.get_num_elements(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// Close when escape is pressed
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if(action == GLFW_PRESS) {
		keys[key] = true;
	} else if(action == GLFW_RELEASE) {
		keys[key] = false;
	}
}

void key_parse() {
	if(keys[GLFW_KEY_W]) {
		camera_y += 4;
	} if(keys[GLFW_KEY_A]) {
		camera_x -= 4;
	} if(keys[GLFW_KEY_S]) {
		camera_y -= 4;
	} if(keys[GLFW_KEY_D]) {
		camera_x += 4;
	}
}
