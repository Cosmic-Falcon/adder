#ifndef GEO_POLYGON_H
#define GEO_POLYGON_H

#include <array>
#include <queue>
#include <vector>

#include "gfx.h"

using vertex_t = std::array<GLfloat, 2>;
using vertices_t = std::vector<vertex_t>;

class Polygon {
public:
	Polygon(vertices_t vertices, GLfloat x=0, GLfloat y=0);
	~Polygon();

	GLfloat* get_vertices();
	GLuint* get_indices();
	int get_vertices_size();
	int get_indices_size();
	int get_num_elements();
private:
	vertices_t get_subpolygon(int &top_start, int top_end, int bottom_start, int &bottom_end);
	std::vector<vertices_t> partition();
	void triangulate(vertices_t &vertices, int &start_index, int &indices_index);
	void gen_gl_data();

	GLfloat x;
	GLfloat y;
	vertices_t vertices;

	// Cache data
	bool cache_cur = false; // True if the cache is current
	GLfloat* gl_vertices;
	GLuint* gl_indices;
	int vertices_size;
	int indices_size;
	int num_elements;
};

#endif // GEO_POLYGON_H
