#ifndef VECTOR_FUNCTIONS_H
#define VECTOR_FUNCTIONS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "adder_global.h"

namespace adder {

glm::vec4 normal(const glm::vec4 &vec);

float scalar_project(const glm::vec2 &vec, const glm::vec2 &axis);
glm::vec2 project(const glm::vec2 &vec, const glm::vec2 &axis);

} // adder

#endif // VECTOR_FUNCTIONS_H
