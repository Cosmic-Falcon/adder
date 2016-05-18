#include "polygon.h"

Polygon::Polygon(std::vector<glm::vec4> vertices, glm::vec4 pos) :
	vertices{vertices} {
	set_position(pos);
	gl_vertices = nullptr;
	gl_indices = nullptr;
	std::cout << to_string(vertices) << std::endl;
}

Polygon::~Polygon() {
	delete[] gl_vertices;
	delete[] gl_indices;
}

void Polygon::rotate(float ang, const glm::vec4 &axis) {
	if(ang == 0) return;
	glm::vec4 negated_axis;
	negated_axis = -axis;
	negated_axis[3] = 1;
	translate(negated_axis); 
	glm::mat4 rotmat = glm::rotate(glm::mat4(1.0), (float) ang*180/PI,glm::vec3(0, 0, 1)); // Always rotating around z-axis
	for (int i = 0; i < vertices.size(); ++i)
		vertices[i] = rotmat*vertices[i];
	translate(axis);
	cache_cur = false;
}

void Polygon::set_position(const glm::vec4 &pos) {
	if(pos != this->pos) {
		translate(pos - this->pos);
		cache_cur = false;
	}
}

void Polygon::translate(const glm::vec4 &xy) {
	pos += xy;
	pos[3] = 1;
	for(glm::vec4 &pt : vertices) {
		pt += xy;
		pt[3] = 1;
	}
	cache_cur = false;
}

GLfloat* Polygon::get_vertices() {
	gen_gl_data();
	return gl_vertices;
}

GLuint* Polygon::get_indices() {
	gen_gl_data();
	return gl_indices;
}

int Polygon::get_vertices_size() {
	gen_gl_data();
	return vertices_size;
}

int Polygon::get_indices_size() {
	gen_gl_data();
	return indices_size;
}

int Polygon::get_num_elements() {
	return num_elements;
}

glm::vec4 Polygon::get_pos() {
	return pos;
}

int constrain(int index, int bound) {
	while(index > bound) index -= bound;
	while(index < 0) index += bound;
	return index;
}

void Polygon::gen_gl_data() {
	if(!cache_cur) {
		int num_vertices = vertices.size();
		num_elements = (num_vertices - 2) * 3;

		if(gl_vertices) delete[] gl_vertices;
		if(gl_indices) delete[] gl_indices;
		gl_vertices = new GLfloat[num_vertices * 3];
		gl_indices = new GLuint[(num_vertices - 2) * 3];
		int indices_index = 0; // Index of gl_indices to add to

		int left_index = 0; // Index of the leftmost point (start point)
		int right_index = 0; // Index of the rightmost point (end point)

		for(int i = 0; i < num_vertices; ++i) {
			// Format vertices for OpenGL
			gl_vertices[i * 3] = vertices[i][0];
			gl_vertices[i * 3 + 1] = vertices[i][1];
			gl_vertices[i * 3 + 2] = 0.0f;

			// Find leftmost and rightmost vertices
			if(vertices[i][0] < vertices[left_index][0])
				left_index = i;
			else if(vertices[i][0] > vertices[right_index][0])
				right_index = i;
		}

		DEBUG("Left: " << left_index << "; Right: " << right_index << std::endl);

		int a = left_index; // Index of top vertex
		int b = left_index; // Index of bottom vertex
		int current = left_index; // Vertex being analyzed
		int last = left_index; // Last vertex to be analyzed

		std::vector<int> remaining_vertices;
		remaining_vertices.push_back(left_index);

		// Sweep through vertices from left to right and triangulate each monotone polygon
		for(int i = 0; i < num_vertices; ++i) {
			// Move to next vertex to the right to analyze
			DEBUG("INITIAL A: " << a << "; B: " << b);
			DEBUG("TEST A: " << constrain(a + 1, num_vertices) << "; B: " << constrain(b - 1, num_vertices));
			DEBUG("A: (" << vertices[constrain(a + 1, num_vertices)][0] << ", " << vertices[constrain(a + 1, num_vertices)][1] << ")");
			DEBUG("B: (" << vertices[constrain(b - 1, num_vertices)][0] << ", " << vertices[constrain(b - 1, num_vertices)][1] << ")");
			DEBUG("A.x < B.x: " << ((vertices[constrain(a + 1, num_vertices)][0] < vertices[constrain(b - 1, num_vertices)][0]) == 1 ? "true" : "false"));
			if((vertices[constrain(a + 1, num_vertices)][0] < vertices[constrain(b - 1, num_vertices)][0] || b == right_index) && a != right_index) {
				DEBUG("Moving top vertex");
				a = constrain(a + 1, num_vertices);
				last = current;
				current = a;
			} else {
				DEBUG("Moving bottom vertex");
				b = constrain(b - 1, num_vertices);
				last = current;
				current = b;
			}
			if(last != left_index) remaining_vertices.push_back(last);
			DEBUG("ADDING TO REMAINING VERTICES: " << last);
			DEBUG("FINAL A: " << a << "; B: " << b << std::endl);

			/* If the next vertex is on the bottom half of the polygon and the previous
			 * vertices that don't form triangles are on the top half (or vice-versa),
			 * then the current point and all of the previous points will form a series
			 * of triangles shaped similarly to a chinese fan. If a fan of triangles is
			 * formed, add each of the triangles in the fan
			 */
			if(indices_index < num_elements && a != left_index && b != left_index) {
				if((current == a && current - 1 != last) || (current == b && current + 1 != last)) {
					DEBUG("Fan");
#ifdef DEBUG_MODE
					std::string str = "";
					for(auto i : remaining_vertices) str += std::to_string(i) + " ";
					DEBUG("Remaining vertices: " << str);
#endif

					int num_remaining_vertices = remaining_vertices.size();
					for(int j = 0; j < num_remaining_vertices - 1; ++j) {
						DEBUG("Triangle around point " << remaining_vertices[j]);
						gl_indices[indices_index++] = current;

						// Add vertices in clockwise order
						int vert_a = remaining_vertices.front();
						remaining_vertices.erase(remaining_vertices.begin());
						int vert_b = remaining_vertices.front();
						if(vertices[vert_a][1] < vertices[vert_b][1]) {
							gl_indices[indices_index++] = vert_a;
							gl_indices[indices_index++] = vert_b;
						} else {
							gl_indices[indices_index++] = vert_b;
							gl_indices[indices_index++] = vert_a;
						}

						DEBUG("Resultant indices: " << gl_indices[indices_index - 3] << ", " << gl_indices[indices_index - 2] << ", " << gl_indices[indices_index - 1]);
					}
#ifdef DEBUG_MODE
					std::cout << std::endl;
#endif
				} else { // If a fan is not formed, check if a triangular ear is formed
					double old_slope = double(vertices[constrain(current - 2, num_vertices)][1] - vertices[constrain(current - 1, num_vertices)][1]) / double(vertices[constrain(current - 2, num_vertices)][0] - vertices[constrain(current - 1, num_vertices)][0]);
					double new_slope = double(vertices[constrain(current - 2, num_vertices)][1] - vertices[constrain(current, num_vertices)][1]) / double(vertices[constrain(current - 2, num_vertices)][0] - vertices[constrain(current, num_vertices)][0]);

					if(current == a && new_slope < old_slope) { // Ear on top
						DEBUG("Upper ear around " << current);
						gl_indices[indices_index++] = current;
						gl_indices[indices_index + 2] = remaining_vertices.back();
						remaining_vertices.pop_back();
						gl_indices[indices_index++] = remaining_vertices.back();
						++indices_index;
						DEBUG("Resultant indices: " << gl_indices[indices_index - 3] << ", " << gl_indices[indices_index - 2] << ", " << gl_indices[indices_index - 1] << std::endl);
					} else if(current == b && new_slope > old_slope) { // Ear on bottom
						DEBUG("Lower ear around " << current);
						gl_indices[indices_index++] = current;
						gl_indices[indices_index++] = remaining_vertices.back();
						remaining_vertices.pop_back();
						gl_indices[indices_index++] = remaining_vertices.back();
						DEBUG("Resultant indices: " << gl_indices[indices_index - 3] << ", " << gl_indices[indices_index - 2] << ", " << gl_indices[indices_index - 1] << std::endl);
					}
				}
			}
		}

		vertices_size = sizeof(GLfloat) * num_vertices * 3;
		indices_size = sizeof(GLint) * (num_vertices - 2) * 3;
		cache_cur = true;

#ifdef DEBUG_MODE
		std::string indices = "";
		for(int i = 0; i < (num_vertices - 2) * 3; ++i) {
			indices += "(";
			indices += std::to_string(gl_indices[i]) + ", ";
			indices += std::to_string(gl_indices[++i]) + ", ";
			indices += std::to_string(gl_indices[++i]);
			indices += ") ";
		}
		DEBUG("Indices: " << indices << std::endl);
#endif
	}
}

std::string to_string(const std::vector<glm::vec4> &vertices) {
	std::string str = "";
	for(const glm::vec4 &vertex : vertices)
		str += "(" + std::to_string(vertex[0]) + ", " + std::to_string(vertex[1]) + ") ";
	return str;
}
