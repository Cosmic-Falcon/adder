#ifndef BODY_H
#define BODY_H

#include <vector>
#include <numeric>

#include "polygon.h"

namespace krypt {
	class Body {
	private:
		float _mass;
		float _I;                        // Moment of Intertia, Angular Mass
		Polygon _poly;
		std::vector<glm::vec2> _forces;
		std::vector<float> _torques;
		glm::vec2 _pos;                  // Position
		glm::vec2 _vel;                  // Velocity
		glm::vec2 _ori;                  // Orientation
		glm::vec2 _ome;                  // Angular Velocity
	public:
		void update(float dt);
		void add_force(const glm::vec2 &force);
		void add_torque(const float &torque);

		float mass() const { return _mass; };
		float angular_mass() const { return _I; };
		glm::vec2 position() const { return _pos; };
		glm::vec2 velocity() const { return _vel; };
		glm::vec2 orientation() const { return _ori; };
		glm::vec2 angular_velocity() const { return _ome; };
	};
}

#endif
