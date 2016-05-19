#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef DEBUG_MODE
#define DEBUG(msg) std::cout << "DEBUG (" << __FILE__ << ":" << __LINE__ << "): " << msg << std::endl;
#else
#define DEBUG(msg)
#endif

#define ERROR(msg) std::cerr << "ERROR! (" << __FILE__ << ":" << __LINE__ << "): " << msg << std::endl;

const float PI = glm::pi<float>();
const float UNIV_G = 6.6740831313131 * std::pow(10, -3); // Universal Gravitational Constant (x10^-11)

#endif // GLOBAL_H
