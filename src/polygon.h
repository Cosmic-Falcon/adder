#ifndef GEO_POLYGON_H
#define GEO_POLYGON_H

#include <algorithm>
#include <array>
#include <queue>
#include <vector>

#include "gfx.h"
#include "adder_constants.h"
#include "vector_fns.h"

namespace adder {

class Polygon {
public:
	using Vertices = std::vector<glm::vec3>;
	Polygon(Vertices vertices, glm::vec3 pos = glm::vec3{0.f, 0.f, 0.f});
	~Polygon();

	void rotate(float ang, const glm::vec3 &axis);
	void set_position(const glm::vec3 &pos);
	void translate(const glm::vec3 &xy);

	glm::vec3 position();
	Vertices vertices();

	bool is_convex();
	GLfloat* get_gl_vertices();
	GLuint* get_gl_indices();
	int get_gl_vertices_size();
	int get_gl_indices_size();

	int get_num_elements();
	glm::vec3 get_position();
private:
	std::vector<int> get_subpolygon(int &top_start, int top_end, int bottom_start, int &bottom_end);
	std::vector<std::vector<int>> partition();
	void triangulate(std::vector<int> &indices, int &start_index, int &indices_index);
	void gen_gl_data();

	glm::vec3 _pos;
	Vertices _verts;

	// Cache data
	struct CacheStatus {
		// True if the cache is up to date
		bool gl_data = false;
		// True if concavity has been determined
		bool is_convex = false;
	} _cache_status;

	GLfloat* _gl_verts;
	GLuint* _gl_indices;
	int _verts_size; // Size of _gl_verts in bytes
	int _indices_size; // Size of _gl_indices in bytes
	int _num_elmns; // Number of elements
	bool _is_convex;
};

} // adder

std::string to_string(const std::vector<glm::vec3> &vertices);

#endif // GEO_POLYGON_H
