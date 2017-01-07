#ifndef BODY_H
#define BODY_H

#include <vector>

#include "polygon.h"

namespace adder {

class Body {
private:
	float _mass;
	float _I;                        // Moment of Intertia, Angular Mass
	float _q;			             // Charge
	Polygon _poly;
	std::vector<glm::vec2> _forces;
	std::vector<float> _torques;
	glm::vec2 _pos;                  // Position
	glm::vec2 _vel;                  // Velocity
	float _ang;                  // Orientation/Angle
	float _ang_v;                // Angular Velocity

	std::vector<glm::vec2> _seperation_axes; //Sets of Axes on which this body is tested for collision with other bodies
	std::vector<glm::vec2> _gen_sep_axes();
public:
	Body(float mass, float moment_of_inertia, float charge, Polygon poly);
	Body(float mass, float moment_of_inertia, float charge, std::vector<glm::vec2> vertices, glm::vec2 pos = glm::vec2{0.f, 0.f});
	
	void update(float dt);
	void add_force(const glm::vec2 &force);
	void add_torque(const float &torque);
	void set_velocity(const glm::vec2 &vel);
	void set_angular_velocity(const float &ang_v);

	float mass() const { return _mass; };
	float moment_of_inertia() const { return _I; };
	float charge() const { return _q; };
	glm::vec2 position() const { return _pos; };
	glm::vec2 velocity() const { return _vel; };
	float orientation() const { return _ang; };
	float angular_velocity() const { return _ang_v; };

	Polygon get_poly() const { return _poly; };
	std::vector<glm::vec2> seperation_axes() const { return _seperation_axes; }
};

} // adder

#endif // BODY_H
