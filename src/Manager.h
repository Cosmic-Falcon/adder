#ifndef MANAGER_H
#define MANAGER_H

#include <map>
#include <string>
#include <memory>

#include "Entity.h"

namespace adder {

class Manager {
public:

	void update(float dt);

	Entity* get(std::string key);
	std::map<std::string, Entity*> entitites() const { return _entities; }
	// Return true if pair added, false if not
	bool add_entity(std::pair<std::string, Entity*> pair);
	bool add_entity(std::string key, Entity *entity);
	bool add_entity(Entity *entity);

	glm::vec2 projection_axis = {0, 0};
private:
	std::map<std::string, Entity*> _entities;
	unsigned int _latest_id = 0;

	// Return: (if entities a and b have collided, shortest vector to move them out of collision)
	std::pair<bool, glm::vec2> collided(Entity *a, Entity *b);
	
	// Each input pair is a range, [min, max]
	// Result: (whether overlaps or not, distance overlapped)
	float overlap(std::pair<float, float> a, std::pair<float, float> b);
};

} // end adder


#endif