#include "polygon.h"

Polygon::Polygon(vertices_t vertices, GLfloat x, GLfloat y) {
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

int Polygon::get_num_elements() {
	return num_elements;
}

int constrain(int index, int bound) {
	while (index >= bound) index -= bound;
	while (index < 0) index += bound;

	return index;
}

vertices_t Polygon::get_subpolygon(int &top_start, int top_end, int bottom_start, int &bottom_end) {
	DEBUG("SUBPOLYGON: " << top_start << ", " << top_end << ", " << bottom_start << ", " << bottom_end);
	int num_vertices = vertices.size();
	vertices_t subpolygon;

	for (int i = top_start; i != top_end; i = constrain(i + 1, num_vertices)) {
		DEBUG(i);
		subpolygon.push_back(vertices[i]);
	}
	subpolygon.push_back(vertices[top_end]);

	for (int i = bottom_start; i != bottom_end; i = constrain(i + 1, num_vertices)) {
		DEBUG(i);
		subpolygon.push_back(vertices[i]);
	}
	subpolygon.push_back(vertices[bottom_end]);

	top_start = top_end;
	bottom_end = bottom_start;

	DEBUG(top_start);
	DEBUG(bottom_end);

	return subpolygon;
}

std::vector<vertices_t> Polygon::partition() {
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
	DEBUG("LEFT TO RIGHT:");
	for (int i = 0; i < num_vertices; ++i) {
		DEBUG(node->index);
		node = node->right;
	}
#endif

	std::vector<vertices_t> partitions;

	current = root;
	int start_index = left_index;
	int close_index = constrain(left_index - 1, num_vertices);
	DEBUG("PARTITIONING INTO MONOTONE SUBPOLYGONS");
	for (int i = 0; i < num_vertices; ++i) {
		int index = current->index;

		if (index != left_index && vertices[index][0] < vertices[constrain(index - 1, num_vertices)][0] && vertices[index][0] < vertices[constrain(index + 1, num_vertices)][0]) {
			DEBUG("Split creates: " << start_index << ", " << index << ", " << current->left->index << ", " << close_index);
			partitions.push_back(get_subpolygon(start_index, index, current->left->index, close_index));
		} else if (index != right_index && vertices[i][0] > vertices[constrain(index - 1, num_vertices)][0] && vertices[i][0] > vertices[constrain(index + 1, num_vertices)][0]) {
			DEBUG("Merge creates: " << start_index << ", " << index << ", " << current->right->index << ", " << close_index);
			partitions.push_back(get_subpolygon(start_index, index, current->right->index, close_index));
		}

		if (current == rightmost)
			break;
		else
			current = current->right;
	}

	DEBUG("Final subpolygon creation: " << start_index << ", " << right_index << ", " << constrain(right_index + 1, num_vertices) << ", " << close_index);
	partitions.push_back(get_subpolygon(start_index, right_index, constrain(right_index + 1, num_vertices), close_index));

	std::cout << partitions.size() << std::endl;
	for (vertices_t partition : partitions) {
		for (int i = 0; i < partition.size(); ++i) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
	}

	return partitions;
}

void Polygon::triangulate(vertices_t &vertices, int &start_index, int &indices_index) {
	int num_vertices = vertices.size();

#ifdef DEBUG_MODE
		std::string vertex_string = "";
		for (int i = 0; i < num_vertices; ++i) {
			vertex_string += std::to_string(vertices[i][0]) + ", " + std::to_string(vertices[i][1]) + " ";
		}
		DEBUG("Triangulating " + vertex_string);
#endif

	int left_index = 0; // Index of the leftmost point (start point)
	int right_index = 0; // Index of the rightmost point (end point)

	for (int i = 0; i < num_vertices; ++i) {
		// Format vertices for OpenGL
		gl_vertices[(i + start_index) * 3] = vertices[i][0];
		gl_vertices[(i + start_index) * 3 + 1] = vertices[i][1];
		gl_vertices[(i + start_index) * 3 + 2] = 0.0f;
		
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

	std::vector<int> remaining_vertices;
	remaining_vertices.push_back(left_index);

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
		if (last != left_index) remaining_vertices.push_back(last);
		DEBUG("ADDING TO REMAINING VERTICES: " << last);
		DEBUG("FINAL A: " << a << "; B: " << b << std::endl);

		/* If the next vertex is on the bottom half of the polygon and the previous
		 * vertices that don't form triangles are on the top half (or vice-versa),
		 * then the current point and all of the previous points will form a series
		 * of triangles shaped similarly to a chinese fan. If a fan of triangles is
		 * formed, add each of the triangles in the fan
		 */
		if (indices_index < num_elements && a != left_index && b != left_index) {
			if ((current == a && current-1 != last) || (current == b && current+1 != last)) {
				DEBUG("Fan");
#ifdef DEBUG_MODE
					std::string str = "";
					for (auto i : remaining_vertices) str += std::to_string(i) + " ";
					DEBUG("Remaining vertices: " << str);
#endif

				int num_remaining_vertices = remaining_vertices.size();
				for (int j = 0; j < num_remaining_vertices - 1; ++j) {
					DEBUG("Triangle around point " << remaining_vertices[j]);
					gl_indices[indices_index++] = current;

					// Add vertices in clockwise order
					int vert_a = remaining_vertices.front();
					remaining_vertices.erase(remaining_vertices.begin());
					int vert_b = remaining_vertices.front();
					if (vertices[vert_a][1] < vertices[vert_b][1]) {
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
				double old_slope = double(vertices[constrain(current-2, num_vertices)][1] - vertices[constrain(current-1, num_vertices)][1]) / double(vertices[constrain(current-2, num_vertices)][0] - vertices[constrain(current-1, num_vertices)][0]);
				double new_slope = double(vertices[constrain(current-2, num_vertices)][1] - vertices[constrain(current, num_vertices)][1]) / double(vertices[constrain(current-2, num_vertices)][0] - vertices[constrain(current, num_vertices)][0]);

				if (current == a && new_slope < old_slope) { // Ear on top
					DEBUG("Upper ear around " << current);
					gl_indices[indices_index++] = current;
					gl_indices[indices_index + 2] = remaining_vertices.back();
					remaining_vertices.pop_back();
					gl_indices[indices_index++] = remaining_vertices.back();
					++indices_index;
					DEBUG("Resultant indices: " << gl_indices[indices_index - 3] << ", " << gl_indices[indices_index - 2] << ", " << gl_indices[indices_index - 1] << std::endl);
				} else if (current == b && new_slope > old_slope) { // Ear on bottom
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
}

void Polygon::gen_gl_data() {
	if (!cache_cur) {
		int num_vertices = vertices.size();
		num_elements = (num_vertices - 2) * 3;
		
		if (gl_vertices) delete[] gl_vertices;
		if (gl_indices) delete[] gl_indices;
		gl_vertices = new GLfloat[num_vertices * 3];
		gl_indices = new GLuint[(num_vertices - 2) * 3];
		int indices_index = 0; // Index of gl_indices to add to
		int index = 0;

		std::vector<vertices_t> partitions = partition();
		//exit(0); // TODO: REMOVE THIS WHEN PARTITIONING WORKS!
		for (vertices_t vertices : partitions) {
			triangulate(vertices, index, indices_index);
		}

		vertices_size = sizeof(GLfloat) * num_vertices * 3;
		indices_size = sizeof(GLint) * (num_vertices - 2) * 3;
		cache_cur = true;

#ifdef DEBUG_MODE
			std::string indices = "";
			for (int i = 0; i < (num_vertices - 2) * 3; ++i) {
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
