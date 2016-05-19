#include "vector_fns.h"

glm::vec4 normal(const glm::vec4 &vec) {
	static glm::mat4 rot_mat = glm::rotate(glm::mat4(1.0), 90.f, glm::vec3(0, 0, 1));
	return rot_mat*vec;
}
