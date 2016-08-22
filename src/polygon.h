#ifndef GEO_POLYGON_H
#define GEO_POLYGON_H

#include <algorithm>
#include <array>
#include <queue>
#include <vector>

#include <boa/boa.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "adder_global.h"

namespace adder {

using Vertices = std::vector<glm::vec4>;

class Polygon {
public:
	Polygon(std::vector<glm::vec4> vertices, glm::vec4 pos = glm::vec4{0.f, 0.f, 0.f, 1.f});
	~Polygon();

	void rotate(float ang, const glm::vec4 &axis);
	void set_position(const glm::vec4 &pos);
	void translate(const glm::vec4 &xy);

	glm::vec4 position();
	std::vector<glm::vec4> vertices();

	bool is_convex();
	glm::vec4 get_pos();
private:
	void gen_gl_data();

	glm::vec4 _pos;
	std::vector<glm::vec4> _verts;

	// Cache data
	struct CacheStatus {
		// True if the cache is up to date
		bool gl_data = false;
		bool is_convex = false;
	} _cache_status;

	bool _is_convex;
};

} // adder

std::string to_string(const std::vector<glm::vec4> &vertices);

#endif // GEO_POLYGON_H
