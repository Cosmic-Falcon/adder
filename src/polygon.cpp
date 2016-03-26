#include "polygon.h"

Polygon::Polygon(std::vector<std::array<GLfloat, 2>> vertices, GLfloat x, GLfloat y) {
	this->x = x;
	this->y = y;
	this->vertices = vertices;

	gl_vertices = nullptr;
	gl_indices = nullptr;
}

Polygon::~Polygon() {
	delete[] gl_vertices;
	delete[] gl_indices;
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

int constrain(int index, int bound) {
	int value = index;
	while (value > bound) value -= bound;
	while (value < 0) value += bound;

	return value;
}

void Polygon::gen_gl_data() {
	if (!cache_cur) {
		int num_vertices = vertices.size();
		
		if (gl_vertices) delete[] gl_vertices;
		if (gl_indices) delete[] gl_indices;
		gl_vertices = new GLfloat[num_vertices * 3];
		gl_indices = new GLuint[(num_vertices - 2) * 3];
		int indices_index = 0; // Index of gl_indices to add to

		int left_index = 0; // Index of the leftmost point (start point)
		int right_index = 0; // Index of the rightmost point (end point)

		for (int i = 0; i < num_vertices; ++i) {
			// Format vertices for OpenGL
			gl_vertices[i * 3] = vertices[i][0];
			gl_vertices[i * 3 + 1] = vertices[i][1];
			gl_vertices[i * 3 + 2] = 0.0f;
			
			// Find leftmost and rightmost vertices
			if (vertices[i][0] < vertices[left_index][0])
				left_index = i;
			else if (vertices[i][0] > vertices[right_index][0])
				right_index = i;
		}

		DEBUG("Left: " << left_index << "; Right: " << right_index << std::endl);

		int a = left_index; // Index of top vertex
		int b = left_index; // Index of bottom vertex
		int current = left_index; // Vertex being analyzed
		int last = left_index; // Last vertex to be analyzed

		std::vector<int> prev_vertices;
		prev_vertices.push_back(0);

		// Sweep through vertices from left to right and triangulate each monotone polygon
		for (int i = 0; i < num_vertices; ++i) {
			// Move to next vertex to the right to analyze
			DEBUG("INITIAL A: " << a << "; B: " << b);
			DEBUG("TEST A: " << constrain(a+1, num_vertices) << "; B: " << constrain(b-1, num_vertices));
			DEBUG("A: (" << vertices[constrain(a+1, num_vertices)][0] << ", " << vertices[constrain(a+1, num_vertices)][1] << ")");
			DEBUG("B: (" << vertices[constrain(b-1, num_vertices)][0] << ", " << vertices[constrain(b-1, num_vertices)][1] << ")");
			DEBUG("A.x < B.x: " << ((vertices[constrain(a+1, num_vertices)][0] < vertices[constrain(b-1, num_vertices)][0]) == 1 ? "true" : "false"));
			if ((vertices[constrain(a+1, num_vertices)][0] < vertices[constrain(b-1, num_vertices)][0] || b == right_index) && a != right_index) {
				DEBUG("Moving top vertex");
				a = constrain(a+1, num_vertices);
				last = current;
				current = a;
			} else {
				DEBUG("Moving bottom vertex");
				b = constrain(b-1, num_vertices);
				last = current;
				current = b;
			}
			if (last != 0) prev_vertices.push_back(last);
			DEBUG("FINAL A: " << a << "; B: " << b << std::endl);

			if (a != left_index && b != left_index && a != b) {
				/* If the next vertex is on the bottom half of the polygon and the previous
				 * vertices that don't form triangles are on the top half (or vice-versa),
				 * then the current point and all of the previous points will form a series
				 * of triangles shaped similarly to a chinese fan. If a fan of triangles is
				 * formed, add each of the triangles in the fan
				 */
				if (current-1 != last) {
					DEBUG("Fan");
					int num_prev_vertices = prev_vertices.size();
					for (int j = 0; j < num_prev_vertices - 1; ++j) {
						DEBUG("Triangle around point " << prev_vertices[j]);
						gl_indices[indices_index++] = current;

						// Add vertices in clockwise order
						int vert_a = prev_vertices.front();
						prev_vertices.erase(prev_vertices.begin());
						int vert_b = prev_vertices.front();
						if (vertices[vert_a][1] < vertices[vert_b][1]) {
							gl_indices[indices_index++] = vert_a;
							gl_indices[indices_index++] = vert_b;
						} else {
							gl_indices[indices_index++] = vert_b;
							gl_indices[indices_index++] = vert_a;
						}

						DEBUG("Resultant indices: " << gl_indices[indices_index - 3] << ", " << gl_indices[indices_index - 2] << ", " << gl_indices[indices_index - 1]);
					}
					std::cout << std::endl;
				} else { // If a fan is not formed, check if a triangular ear is formed
					double old_slope = double(vertices[current-2][1] - vertices[current-1][1]) / double(vertices[current-2][0] - vertices[current-1][0]);
					double new_slope = double(vertices[current-2][1] - vertices[current][1]) / double(vertices[current-2][0] - vertices[current][0]);

					if (current == a && new_slope < old_slope) { // Ear on top
						DEBUG("Upper ear" << std::endl);
						gl_indices[indices_index++] = current;
						gl_indices[indices_index + 2] = prev_vertices.back();
						prev_vertices.pop_back();
						gl_indices[indices_index++] = prev_vertices.back();
						++indices_index;
					} else if (current == b && new_slope > old_slope) { // Ear on bottom
						DEBUG("Lower ear" << std::endl);
						gl_indices[indices_index++] = current;
						gl_indices[indices_index++] = prev_vertices.back();
						prev_vertices.pop_back();
						gl_indices[indices_index++] = prev_vertices.back();
					}
				}
			}
		}

		vertices_size = sizeof(GLfloat) * num_vertices * 3;
		indices_size = sizeof(GLint) * (num_vertices - 2) * 3;
		cache_cur = true;
	}
}
