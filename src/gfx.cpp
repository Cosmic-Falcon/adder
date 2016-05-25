#include "gfx.h"

namespace gfx {

bool init(GLint version_major, GLint version_minor, GLboolean resizable) {
	// Initialize GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version_minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	glewExperimental = GL_TRUE;

	return true; // TODO: Should return false if initialization fails
}


// Shaders
GLuint compile_shader(const char* source, GLenum type) {
	std::ifstream shader_file;
	shader_file.open(source, std::ios::in);
	std::string shader_source;
	if(shader_file.is_open()) {
		std::string line = "";
		while(std::getline(shader_file, line)) {
			shader_source += line + "\n";
		}

		shader_file.close();
	}

	const char* shader_c_str = shader_source.c_str();

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_c_str, NULL);
	glCompileShader(shader);

	GLint success;
	GLchar info_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		ERROR(source << ":" << info_log);

		return -1;
	}

	return shader;
}

GLuint create_program(std::initializer_list<GLuint> shaders) {
	GLuint program = glCreateProgram();
	for(GLuint shader : shaders) {
		glAttachShader(program, shader);
	}
	glLinkProgram(program);

	return program;
}


// Textures
GLuint load_texture(const char* source) {
	int width, height;
	unsigned char* image = SOIL_load_image(source, &width, &height, 0, SOIL_LOAD_RGB);

	GLuint texture;
	glGenTextures(1, &texture);

	if(image == NULL) {
		ERROR("Image " << source << " does not exist");
		return texture;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	//           Internal info                               External info
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(image);

	return texture;
}

void set_texture(GLenum target, GLenum unit, GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(target, texture);
}


// Windows
GLFWwindow* create_window(int width, int height, std::string name, GLFWmonitor* monitor, GLFWwindow* share) {
	GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), monitor, share);
	if(window == nullptr) {
		ERROR("Failed to create window \"" << name << "\"");
	}

	glfwMakeContextCurrent(window);

	if(glewInit() != GLEW_OK) {
		ERROR("Failed to initialize GLEW for window \"" << name << "\"");
	}

	return window;
}

} // gfx