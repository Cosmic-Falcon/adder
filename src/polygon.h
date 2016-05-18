#ifndef GEO_POLYGON_H
#define GEO_POLYGON_H

#include <algorithm>
#include <array>
#include <queue>
#include <vector>

#include "gfx.h"

using Vertices = std::vector<std::array<GLfloat, 2>>;

class Polygon {
public:
	Polygon(Vertices vertices, GLfloat x=0, GLfloat y=0);
	~Polygon();

	GLfloat* get_vertices();
	GLuint* get_indices();
	int get_vertices_size();
	int get_indices_size();
	int get_num_elements();
private:
	std::vector<int> get_subpolygon(int &top_start, int top_end, int bottom_start, int &bottom_end);
	std::vector<std::vector<int>> partition();
	void triangulate(std::vector<int> &indices, int &start_index, int &indices_index);
	void gen_gl_data();

	GLfloat x;
	GLfloat y;
	Vertices vertices;

	// Cache data
	bool cache_cur = false; // True if the cache is current
	GLfloat* gl_vertices;
	GLuint* gl_indices;
	int vertices_size;
	int indices_size;
	int num_elements;
};

#endif // GEO_POLYGON_H
