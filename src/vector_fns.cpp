#include "vector_fns.h"

namespace adder {
glm::vec4 normal(const glm::vec4 &vec) {
	static glm::mat4 rot_mat = glm::rotate(glm::mat4(1.0), 90.f, glm::vec3(0, 0, 1));
	return rot_mat*vec;
}

float scalar_project(const glm::vec2 &vec, const glm::vec2 &axis) {
	return (glm::dot(vec, axis) / std::pow(glm::length(axis), 2));
}

glm::vec2 project(const glm::vec2 &vec, const glm::vec2 &axis) {
	return axis*scalar_project(vec, axis);
}

} // adder