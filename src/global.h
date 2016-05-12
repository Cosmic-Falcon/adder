#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef DEBUG_MODE
#define DEBUG(msg) std::cout << "DEBUG (" << __FILE__ << ":" << __LINE__ << "): " << msg << std::endl;
#else
#define DEBUG(msg)
#endif

#define ERROR(msg) std::cerr << "ERROR! (" << __FILE__ << ":" << __LINE__ << "): " << msg << std::endl;

const float PI = glm::pi<float>(); 
#endif // GLOBAL_H
