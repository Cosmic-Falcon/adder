#include "Manager.h"
#include <iostream>
#include <algorithm>

namespace adder {
void Manager::update(float dt) {
	for(auto &pair : _entities) {
		pair.second->cycle(dt);
	}
	for(auto i = _entities.begin(); i != _entities.end(); ++i) {
		for(auto j = std::next(i,1); j != _entities.end(); ++j) {
			bool have_collided;
			glm::vec2 axis;
			std::tie(have_collided, axis) = collided(i->second, j->second);
			if(have_collided) {
				std::cout << "collision" << std::endl;
				projection_axis = axis;
			}
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
std::pair<bool, glm::vec2> Manager::collided(Entity *a, Entity *b) {
	auto sep_axes = a->body().seperation_axes();
	auto sep_axis_it = sep_axes.begin();
	float min_overlap = overlap(a->body().project_onto(*sep_axis_it), b->body().project_onto(*sep_axis_it));
	auto min_dist_sep_ax = *sep_axis_it;
	++sep_axis_it;
	for(; sep_axis_it != sep_axes.end(); ++sep_axis_it) {
		float len_overlap = overlap(a->body().project_onto(*sep_axis_it), b->body().project_onto(*sep_axis_it));
		if(len_overlap == 0)
			return{false, {0, 0}};
		if(len_overlap < min_overlap) {
			min_overlap = len_overlap;
			min_dist_sep_ax = *sep_axis_it;
		}
	}
	return{true, min_overlap*glm::normalize(min_dist_sep_ax)};
}
float Manager::overlap(std::pair<float, float> a, std::pair<float, float> b) {
	float a_min = a.first, a_max = a.second;
	float b_min = b.first, b_max = b.second;
	if(b_min > a_max || a_min > b_max)
		return 0;
	auto is_subset = [] (std::pair<float, float> a, std::pair<float, float> b) {
		return (a.first >= b.first && a.second <= b.second);
	};
	if(is_subset(a, b))
		return a_max - a_min;
	if(is_subset(b, a))
		return b_max - b_min;
	return std::min(a_max - b_min, b_max - a_min);
}
} // end adder