#ifndef ADDER_GLOBAL_H
#define ADDER_GLOBAL_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace adder {

// Constants
const float PI = glm::pi<float>();
const float UNIV_G = 0.000000000066740831313131; // Universal Gravitational Constant

// Macros
#ifdef DEBUG_MODE
#define DEBUG(msg) std::cout << "DEBUG (" << __FILE__ << ":" << __LINE__ << "): " << msg << std::endl;
#else
#define DEBUG(msg)
#endif

#define ERROR(msg) std::cerr << "ERROR! (" << __FILE__ << ":" << __LINE__ << "): " << msg << std::endl;

} // adder

#endif // ADDER_GLOBAL_H
