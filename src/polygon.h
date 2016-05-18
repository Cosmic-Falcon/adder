#ifndef GEO_POLYGON_H
#define GEO_POLYGON_H

#include <algorithm>
#include <array>
#include <queue>
#include <vector>

#include "gfx.h"

using Vertices = std::vector<glm::vec4>;

class Polygon {
public:
	using vec4 = glm::vec4;
	Polygon(std::vector<vec4> vertices, glm::vec4 pos = glm::vec4{0.f, 0.f, 0.f, 1.f});
	~Polygon();
	void rotate(float ang, const vec4 &axis); 
	void set_position(const vec4 &pos);
	void translate(const vec4 &xy);
	GLfloat* get_vertices();
	GLuint* get_indices();
	int get_vertices_size();
	int get_indices_size();
	int get_num_elements();
	vec4 get_pos();
private:
	std::vector<int> get_subpolygon(int &top_start, int top_end, int bottom_start, int &bottom_end);
	std::vector<std::vector<int>> partition();
	void triangulate(std::vector<int> &indices, int &start_index, int &indices_index);
	void gen_gl_data();

	glm::vec4 pos;
	std::vector<vec4> vertices;
	// Cache data
	bool cache_cur = false; // True if the cache is current
	GLfloat* gl_vertices;
	GLuint* gl_indices;
	int vertices_size;
	int indices_size;
	int num_elements;
};

std::string to_string(const std::vector<glm::vec4> &vertices);

#endif // GEO_POLYGON_H
