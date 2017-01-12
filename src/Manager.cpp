#include "Manager.h"
#include <iostream>
namespace adder {
void Manager::update(float dt) {
	for(auto &pair : _entities) {
		pair.second->cycle(dt);
	}
	for(auto a : _entities) {
		for(auto b : _entities) {
			if(a != b && collided(a.second, b.second))
				std::cout << "collision" << std::endl;
		}
	}
}
Entity* Manager::get(std::string key) {
	return _entities[key];
}
bool Manager::add_entity(std::pair<std::string, Entity*> pair) {
	return add_entity(pair.first, pair.second);
}
bool Manager::add_entity(std::string key, Entity *entity) {
	if(_entities.find(key) == _entities.end()) {
		_entities.insert(std::make_pair(key, entity));
		return true;
	}
	return false;
}
bool Manager::add_entity(Entity *entity) {
	char str[4];
	while(_entities.find(std::string{str}) != _entities.end()) {
		unsigned int *data = (unsigned int*)&str;
		*data = _latest_id;
		++_latest_id;
	}
	_entities.insert(std::make_pair(std::string{str}, entity));
	return true;
}
bool Manager::collided(Entity *a, Entity *b) {
	for(auto sep_axis : a->body().seperation_axes()) {
		if(!overlap(a->body().project_onto(sep_axis), b->body().project_onto(sep_axis))) {
			return false;
		}
	}
	return true;
}
bool Manager::overlap(std::pair<float, float> a, std::pair<float, float> b) {
	float a_min = a.first, a_max = a.second;
	float b_min = b.first, b_max = b.second;
	if(b_min > a_max || a_min > b_max)
		return false;
	return true;
}
} // end adder