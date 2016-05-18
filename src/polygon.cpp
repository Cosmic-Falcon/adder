#include "polygon.h"

Polygon::Polygon(std::vector<glm::vec4> vertices, glm::vec4 pos) :
	vertices{vertices} {
	set_position(pos);
	gl_vertices = nullptr;
	gl_indices = nullptr;
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
	while (index >= bound) index -= bound;
	while (index < 0) index += bound;

	return index;
}

std::vector<int> Polygon::get_subpolygon(int &top_start, int top_end, int bottom_start, int &bottom_end) {
	DEBUG("SUBPOLYGON: " << top_start << ", " << top_end << ", " << bottom_start << ", " << bottom_end);
		int num_vertices = vertices.size();
	std::vector<int> subpolygon;
		
	for (int i = top_start; i != top_end; i = constrain(i + 1, num_vertices)) {
		subpolygon.push_back(i);
	}
	subpolygon.push_back(top_end);

	for (int i = bottom_start; i != bottom_end; i = constrain(i + 1, num_vertices)) {
		subpolygon.push_back(i);
	}
	subpolygon.push_back(bottom_end);

	top_start = top_end;
	bottom_end = bottom_start;

	return subpolygon;
}

std::vector<std::vector<int>> Polygon::partition() {
	int num_vertices = vertices.size();

	struct Node {
		int index;
		Node *prev;
		Node *next;
		Node *left;
		Node *right;

		Node(int index, Node *prev, Node *next, Node *left, Node *right) {
			this->index = index;
			this->prev = prev;
			this->next = next;
			this->left = left;
			this->right = right;
		}
	};

	auto ltr_compare = [&](int &lhs, int &rhs) -> bool { return vertices[lhs][0] > vertices[rhs][0]; };
	std::priority_queue<int, std::vector<int>, decltype(ltr_compare)> ltr_order(ltr_compare);

	// Get leftmost and rightmost vertices and order vertices from left-to-right in ltr_order
	int left_index = 0;
	int right_index = 0;
	ltr_order.push(0);
	for (int i = 1; i < num_vertices; ++i) {
		ltr_order.push(i);
		if (vertices[i][0] < vertices[left_index][0])
			left_index = i;
		else if (vertices[i][0] > vertices[right_index][0])
			right_index = i;
	}

	// Construct linked list of vertices in order of placement in the polygon
	Node *root = new Node(left_index, nullptr, nullptr, nullptr, nullptr);
	Node *rightmost = nullptr;
	Node *current = root;
	for (int i = left_index + 1; i < num_vertices + left_index; ++i) {
		Node *child = new Node(i % num_vertices, current, nullptr, nullptr, nullptr);
		current->next = child;
		current = child;

		if (child->index == right_index)
			rightmost = child;
	}
	current->next = root;
	root->prev = current;

	// Add links to left and right vertices in the linked list
	Node *next = root;
	for (int i = 0; i < num_vertices; ++i) {
		current = next;
		for (int j = next->index; j < ltr_order.top(); ++j)
			next = next->next;
		for (int j = next->index; j > ltr_order.top(); --j)
			next = next->prev;

		current->right = next;
		next->left = current;
		ltr_order.pop();
	}

#ifdef DEBUG_MODE
	Node *node = root;
	std::string ltr_str = "";
	for (int i = 0; i < num_vertices; ++i) {
		ltr_str += std::to_string(node->index) + " ";
		node = node->right;
	}
	DEBUG("LEFT TO RIGHT: " << ltr_str);
#endif

	// Partition into monotone partitions
	std::vector<std::vector<int>> partitions;
	current = root;
	int start_index = left_index;
	int close_index = constrain(left_index - 1, num_vertices);
	for (int i = 0; i < num_vertices; ++i) {
		int index = current->index;

		if (vertices[index][0] > vertices[left_index][0] && vertices[index][0] < vertices[constrain(index - 1, num_vertices)][0] && vertices[index][0] < vertices[constrain(index + 1, num_vertices)][0]) { // Split
			partitions.push_back(get_subpolygon(start_index, std::min(index, current->left->index), std::max(index, current->left->index), close_index));
		} else if (vertices[index][0] < vertices[right_index][0] && vertices[index][0] > vertices[constrain(index - 1, num_vertices)][0] && vertices[index][0] > vertices[constrain(index + 1, num_vertices)][0]) { // Merge
			partitions.push_back(get_subpolygon(start_index, std::min(index, current->right->index), std::max(index, current->right->index), close_index));
		}

		if (current == rightmost)
			break;
		else
			current = current->right;
	}

	partitions.push_back(get_subpolygon(start_index, right_index, constrain(right_index + 1, num_vertices), close_index));

#ifdef DEBUG_MODE
	DEBUG("Created " << partitions.size() << " partitions:");
	for (auto partition : partitions) {
		for (int i = 0; i < partition.size(); ++i) {
			std::cout << partition[i] << " ";
		}
		std::cout << std::endl;
	}
#endif

	return partitions;
}

void Polygon::triangulate(std::vector<int> &indices, int &start_index, int &indices_index) {
	int num_vertices = indices.size();

#ifdef DEBUG_MODE
	std::string vertex_string = "";
	for (int i = 0; i < num_vertices; ++i) {
		vertex_string += std::to_string(indices[i]) + " ";
	}
	DEBUG("Triangulating " + vertex_string);
#endif

		int left_index = 0; // Index of the leftmost point (start point)
		int right_index = 0; // Index of the rightmost point (end point)

	for (int i = 0; i < num_vertices; ++i) {
			// Find leftmost and rightmost vertices
		if (vertices[indices[i]][0] < vertices[indices[left_index]][0])
				left_index = i;
		else if (vertices[indices[i]][0] > vertices[indices[right_index]][0])
				right_index = i;
		}

		int a = left_index; // Index of top vertex
		int b = left_index; // Index of bottom vertex
		int current = left_index; // Vertex being analyzed
		int last = left_index; // Last vertex to be analyzed

		std::vector<int> remaining_vertices;
		remaining_vertices.push_back(left_index);

		// Sweep through vertices from left to right and triangulate each monotone polygon
		for(int i = 0; i < num_vertices; ++i) {
			// Move to next vertex to the right to analyze
		if ((vertices[indices[constrain(a+1, num_vertices)]][0] < vertices[indices[constrain(b-1, num_vertices)]][0] || b == right_index) && a != right_index) {
			a = constrain(a+1, num_vertices);
				last = current;
				current = a;
			} else {
			b = constrain(b-1, num_vertices);
				last = current;
				current = b;
			}
		if (last != left_index) remaining_vertices.push_back(last);

			/* If the next vertex is on the bottom half of the polygon and the previous
			 * vertices that don't form triangles are on the top half (or vice-versa),
			 * then the current point and all of the previous points will form a series
			 * of triangles shaped similarly to a chinese fan. If a fan of triangles is
			 * formed, add each of the triangles in the fan
			 */
		DEBUG(indices[a] << ", " << indices[b] << ", " << indices[left_index] << " | " << indices_index << ", " << num_elements);
		if (indices_index < num_elements && a != left_index && b != left_index) {
			if ((current == a && current-1 != last) || (current == b && current+1 != last)) {
					DEBUG("Fan");
					#ifdef DEBUG_MODE
						std::string str = "";
				for (auto i : remaining_vertices) str += std::to_string(indices[i]) + " ";
						DEBUG("Remaining vertices: " << str);
					#endif

					int num_remaining_vertices = remaining_vertices.size();
				for (int j = 0; j < num_remaining_vertices - 1; ++j) {
						// Add vertices in clockwise order
						int vert_a = remaining_vertices.front();
						remaining_vertices.erase(remaining_vertices.begin());
						int vert_b = remaining_vertices.front();
					if (current == vert_a || current == vert_b) {
						DEBUG("ABORTING FAN: " << indices[current] << ", " << indices[vert_a] << ", " << indices[vert_b]);
						break;
					}

					gl_indices[indices_index++] = indices[current];
					if (vertices[indices[vert_a]][1] < vertices[indices[vert_b]][1]) {
						gl_indices[indices_index++] = indices[vert_a];
						gl_indices[indices_index++] = indices[vert_b];
						} else {
						gl_indices[indices_index++] = indices[vert_b];
						gl_indices[indices_index++] = indices[vert_a];
						}

						DEBUG("Resultant indices: " << gl_indices[indices_index - 3] << ", " << gl_indices[indices_index - 2] << ", " << gl_indices[indices_index - 1]);
					}
					#ifdef DEBUG_MODE
				str = "";
				for (auto i : remaining_vertices) str += std::to_string(indices[i]) + " ";
				DEBUG("Remaining vertices: " << str);

						std::cout << std::endl;
					#endif
				} else { // If a fan is not formed, check if a triangular ear is formed
				if (std::find(remaining_vertices.begin(), remaining_vertices.end(), constrain(current-2, num_vertices)) != remaining_vertices.end()) {
					double old_slope = double(vertices[indices[constrain(current-2, num_vertices)]][1] - vertices[indices[constrain(current-1, num_vertices)]][1]) /
						double(vertices[indices[constrain(current-2, num_vertices)]][0] - vertices[indices[constrain(current-1, num_vertices)]][0]);
					double new_slope = double(vertices[indices[constrain(current-2, num_vertices)]][1] - vertices[indices[constrain(current, num_vertices)]][1]) /
						double(vertices[indices[constrain(current-2, num_vertices)]][0] - vertices[indices[constrain(current, num_vertices)]][0]);

					if (current == a && new_slope < old_slope) { // Ear on top
						DEBUG("Upper ear around " << indices[current]);
						gl_indices[indices_index++] = indices[current];
						gl_indices[indices_index + 2] = indices[remaining_vertices.back()];
						remaining_vertices.pop_back();
						gl_indices[indices_index++] = indices[remaining_vertices.back()];
						++indices_index;
						DEBUG("Resultant indices: " << gl_indices[indices_index - 3] << ", " << gl_indices[indices_index - 2] << ", " << gl_indices[indices_index - 1] << std::endl);
					} else if (current == b && new_slope > old_slope) { // Ear on bottom
						DEBUG("Lower ear around " << indices[current]);
						gl_indices[indices_index++] = indices[current];
						gl_indices[indices_index++] = indices[remaining_vertices.back()];
						remaining_vertices.pop_back();
						gl_indices[indices_index++] = indices[remaining_vertices.back()];
						DEBUG("Resultant indices: " << gl_indices[indices_index - 3] << ", " << gl_indices[indices_index - 2] << ", " << gl_indices[indices_index - 1] << std::endl);
					}
				}
			}
		}
	}

#ifdef DEBUG_MODE
			std::string indices_str = "";
			for (int i = 0; i < (num_vertices - 2) * 3; ++i) {
				indices_str += "(";
				indices_str += std::to_string(gl_indices[start_index + i]) + ", ";
				indices_str += std::to_string(gl_indices[start_index + (++i)]) + ", ";
				indices_str += std::to_string(gl_indices[start_index + (++i)]);
				indices_str += ") ";
			}
			DEBUG("Partition indices: " << indices_str << std::endl);
#endif

	start_index += num_vertices;
				}

void Polygon::gen_gl_data() {
	if (!cache_cur) {
		int num_vertices = vertices.size();
		num_elements = (num_vertices - 2) * 3;
		
		if (gl_vertices) delete[] gl_vertices;
		gl_vertices = new GLfloat[num_vertices * 3];
		for (int i = 0; i < num_vertices; ++i) {
			// Format vertices for OpenGL
			gl_vertices[(i) * 3] = vertices[i][0];
			gl_vertices[(i) * 3 + 1] = vertices[i][1];
			gl_vertices[(i) * 3 + 2] = 0.0f;
			}

		if (gl_indices) delete[] gl_indices;
		gl_indices = new GLuint[(num_vertices - 2) * 3];
		int indices_index = 0; // Index of gl_indices to add to
		int index = 0;

		std::vector<std::vector<int>> partitions = partition();
		for (std::vector<int> indices : partitions) {
			triangulate(indices, index, indices_index);
		}

		vertices_size = sizeof(GLfloat) * num_vertices * 3;
		indices_size = sizeof(GLint) * (num_vertices - 2) * 3;
		cache_cur = true;

		#ifdef DEBUG_MODE
			std::string indices_str = "";
			for (int i = 0; i < (num_vertices - 2) * 3; ++i) {
				indices_str += "(";
				indices_str += std::to_string(gl_indices[i]) + ", ";
				indices_str += std::to_string(gl_indices[++i]) + ", ";
				indices_str += std::to_string(gl_indices[++i]);
				indices_str += ") ";
			}
			DEBUG("Indices: " << indices_str << std::endl);
		#endif
	}
}

std::string to_string(const std::vector<glm::vec4> &vertices) {
	std::string str = "";
	for(const glm::vec4 &vertex : vertices)
		str += "(" + std::to_string(vertex[0]) + ", " + std::to_string(vertex[1]) + ") ";
	return str;
}
