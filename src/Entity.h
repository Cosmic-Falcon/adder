#ifndef ENTITY_H
#define ENTITY_H

#include "body.h"

namespace adder {
class Entity {
public:
	Entity(Body body) :
		_body(body) {
	}

	// Member function called by Brain/Manager class.
	//	Should not be called by user
	//	Updates body, so body should not be updated by user
	void cycle(float dt) {
		update(dt);
		_body.update(dt);
	}

	// Where User should implement own stuff
	// DO NOT call _body.update, ever
	virtual void update(float dt) {};

	Body &body() { return _body; }
	Body cbody() const { return _body; }
protected:
	Body _body;
};

} //adder
#endif