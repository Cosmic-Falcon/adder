#ifndef ENTITY_H
#define ENTITY_H

#include "polygon.h"

class Entity {
public:
	using RectPoint = std::array<GLfloat, 2>;
	Entity(Polygon polygon, RectPoint pos);
private:
	Polygon polygon;
};

#endif // ENTITY_H