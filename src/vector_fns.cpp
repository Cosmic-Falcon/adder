#include "vector_fns.h"

namespace adder {

glm::vec3 normal(const glm::vec3 &vec) {
	return glm::rotateZ(vec, 90.f);
}

} // adder
