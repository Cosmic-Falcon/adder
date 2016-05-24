#ifndef BODY_H
#define BODY_H

#include <vector>
#include <numeric>

#include "polygon.h"

namespace ape {
	class Body {
	private:
		float _mass;
		float _I;                        // Moment of Intertia, Angular Mass
		float _q;			 // Charge
		Polygon _poly;
		std::vector<glm::vec2> _forces;
		std::vector<float> _torques;
		glm::vec2 _pos;                  // Position
		glm::vec2 _vel;                  // Velocity
		glm::vec2 _ang;                  // Orientation/Angle
		glm::vec2 _ang_v;                // Angular Velocity
	public:
		Body(float mass, float moment_of_inertia, float charge, Polygon poly);
		void update(float dt);
		void add_force(const glm::vec2 &force);
		void add_torque(const float &torque);

		float mass() const { return _mass; };
		float moment_of_inertia() const { return _I; };
		float charge() const { return _q; };
		glm::vec2 position() const { return _pos; };
		glm::vec2 velocity() const { return _vel; };
		glm::vec2 orientation() const { return _ang; };
		glm::vec2 angular_velocity() const { return _ang_v; };
	};
}

#endif // BODY_H
