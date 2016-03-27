#ifndef GEO_POLYGON_H
#define GEO_POLYGON_H

#include <array>
#include <vector>

#include "gfx.h"

class Polygon {
public:
	Polygon(std::vector<std::array<GLfloat, 2>> vertices, GLfloat x=0, GLfloat y=0);
	~Polygon();

	GLfloat* get_vertices();
	GLuint* get_indices();
	int get_vertices_size();
	int get_indices_size();
	int get_num_elements();
private:
	void gen_gl_data();

	GLfloat x;
	GLfloat y;
	std::vector<std::array<GLfloat, 2>> vertices;

	// Cache data
	bool cache_cur = false; // True if the cache is current
	GLfloat* gl_vertices;
	GLuint* gl_indices;
	int vertices_size;
	int indices_size;
	int num_elements;
};

#endif // GEO_POLYGON_H
