#include "body.h"

namespace adder {

Body::Body(float mass, float moment_of_inertia, float charge, Polygon poly) :
	_mass{mass},
	_I{moment_of_inertia},
	_q{charge},
	_poly{poly} {

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
		_forces.reserve(100);
		_torques.push_back(net_torque);
	}
}

void Body::add_force(const glm::vec2 &force) {
	_forces.push_back(force);
}

void Body::add_torque(const float &torque) {
	_torques.push_back(torque);
}

} // adder