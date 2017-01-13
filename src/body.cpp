#include "body.h"

#include <numeric>
#include <algorithm>
#include <iostream>
#include "vector_fns.h"
namespace adder {

std::vector<glm::vec2> Body::_gen_sep_axes() {
	std::vector<glm::vec2> sep_axes;
	auto verts = _poly.vertices();
	for(int i = 0; i < verts.size() - 1; ++i) {
		glm::vec2 line_vec = {verts[i + 1][0] - verts[i][0], verts[i + 1][1] - verts[i][1]};
		glm::vec2 perp_vec = glm::normalize(glm::vec2{line_vec[1], -line_vec[0]});
		if (std::find(sep_axes.begin(), sep_axes.end(), perp_vec) == sep_axes.end()
			&& std::find(sep_axes.begin(), sep_axes.end(), -perp_vec) == sep_axes.end())
			sep_axes.push_back(perp_vec);

	}
	glm::vec2 line_vec = {verts.front()[0] - verts.back()[0], verts.front()[1] - verts.back()[1]};
	glm::vec2 perp_vec = glm::normalize(glm::vec2{line_vec[1], -line_vec[0]});
	if(std::find(sep_axes.begin(), sep_axes.end(), perp_vec) == sep_axes.end()
		&& std::find(sep_axes.begin(), sep_axes.end(), -perp_vec) == sep_axes.end())
		sep_axes.push_back(perp_vec);
	return sep_axes;
}

Body::Body(float mass, float moment_of_inertia, float charge, Polygon poly) :
	_mass{mass},
	_I{moment_of_inertia},
	_q{charge},
	_poly{poly} {
	_seperation_axes = _gen_sep_axes();
}

Body::Body(float mass, float moment_of_inertia, float charge, std::vector<glm::vec2> vertices, glm::vec2 pos) :
	_mass{mass},
	_I{moment_of_inertia},
	_q{charge},
	_pos{pos},
	_poly{vertices, pos} {
	_seperation_axes = _gen_sep_axes();
}

void Body::update(float dt) {
	glm::vec2 net_force = std::accumulate(_forces.begin(), _forces.end(), glm::vec2{0.f, 0.f});
	float net_torque = std::accumulate(_torques.begin(), _torques.end(), 0.f);
	_pos += _vel*dt;
	if(_mass > 0)
		_vel += (net_force / _mass)*dt;
	_ang += _ang_v*dt;
	if(_I > 0)
		_ang_v += (net_torque / _I)*dt;

	_poly.set_position({_pos[0], _pos[1], 0.f, 1.f});
	if(_forces.size() > 100) {
		_forces.clear();
		_forces.reserve(100);
		_forces.push_back(net_force);
	}
	if(_torques.size() > 100) {
		_torques.clear();
		_torques.reserve(100);
		_torques.push_back(net_torque);
	}
}

void Body::add_force(const glm::vec2 &force) {
	_forces.push_back(force);
}

void Body::add_torque(const float &torque) {
	_torques.push_back(torque);
}

void Body::set_velocity(const glm::vec2 &vel) {
	_vel = vel;
}

void Body::set_angular_velocity(const float &ang_v) {
	_ang_v = ang_v;
}

void Body::translate(const glm::vec2 &dpos) {
	_pos += dpos;
	_poly.translate({dpos[0], dpos[1], 0, 0});
}

std::pair<float, float> Body::project_onto(glm::vec2 axis)  {
	glm::vec4 vec4_ax = glm::normalize(glm::vec4{axis[0], axis[1], 0, 0});
	auto mag_sqrd = std::pow(glm::length(vec4_ax), 2);
	float min = glm::dot(_poly.vertices()[0], vec4_ax) / mag_sqrd;
	float max = min;
	for(int i = 1; i < _poly.vertices().size(); ++i) {
		float res = glm::dot(_poly.vertices()[i], vec4_ax) / mag_sqrd;
		if(res > max)
			max = res;
		if(res < min)
			min = res;
	}
	return std::make_pair(min, max);
}

} // adder
