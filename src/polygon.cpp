#include "polygon.h"

namespace adder {

Polygon::Polygon(std::vector<glm::vec3> vertices, glm::vec3 pos) :
	_verts{vertices} {
	set_position(pos);
	_gl_verts = nullptr;
	_gl_indices = nullptr;
}

Polygon::~Polygon() {
	delete[] _gl_verts;
	delete[] _gl_indices;
}

void Polygon::rotate(float ang, const glm::vec3 &axis) {
	if(ang == 0) return;

	translate(-axis);

	for(int i = 0; i < _verts.size(); ++i)
		_verts[i] = glm::rotateZ(_verts[i], ang);

	translate(axis);

	_cache_status.gl_data = false;
}

void Polygon::set_position(const glm::vec3 &pos) {
	if(pos != _pos) {
		translate(pos - _pos);
		_cache_status.gl_data = false;
	}
}

void Polygon::translate(const glm::vec3 &xy) {
	_pos += xy;
	for(glm::vec3 &pt : _verts) {
		pt += xy;
	}
	_cache_status.gl_data = false;
}

glm::vec3 Polygon::position() {
	return _pos;
}

std::vector<glm::vec3> Polygon::vertices() {
	return _verts;
}

bool Polygon::is_convex() {
	if(!_cache_status.is_convex) {
		glm::vec3 a =  _verts.front() -  _verts.back(); //{_verts.front()[0] - _verts.back()[0], _verts.front()[1] - _verts.back()[1], 0.f};
		glm::vec3 b = _verts[1] - _verts.front(); // {_verts[1][0] - _verts.front()[0], _verts[1][1] - _verts.front()[1], 0.f};

		float z = glm::cross(a, b)[2];
		int sign = ((z > 0) ? 1 : -1);

		_is_convex = true;

		for(int i = 0; i < _verts.size() - 2; ++i) {
			glm::vec3 a = _verts[i+1] - _verts[i]; // {_verts[i + 1][0] - _verts[i][0], _verts[i + 1][1] - _verts[i][1], 0.f};
			glm::vec3 b = _verts[i+2] - _verts[i+1]; // {_verts[i + 2][0] - _verts[i + 1][0], _verts[i + 2][1] - _verts[i + 1][1], 0.f};
			float z = glm::cross(a, b)[2];
			int this_sign = ((z > 0) ? 1 : -1);
			if(sign != this_sign) {
				_is_convex = false;
				break;
			}
		}
		_cache_status.is_convex = true;
	}

	return _is_convex;
}

GLfloat* Polygon::get_gl_vertices() {
	gen_gl_data();
	return _gl_verts;
}

GLuint* Polygon::get_gl_indices() {
	gen_gl_data();
	return _gl_indices;
}

int Polygon::get_gl_vertices_size() {
	gen_gl_data();
	return _verts_size;
}

int Polygon::get_gl_indices_size() {
	gen_gl_data();
	return _indices_size;
}

int Polygon::get_num_elements() {
	return _num_elmns;
}

glm::vec3 Polygon::get_position() {
	return _pos;
}
int constrain(int index, int bound) {
	while(index >= bound) index -= bound;
	while(index < 0) index += bound;

	return index;
}

std::vector<int> Polygon::get_subpolygon(int &top_start, int top_end, int bottom_start, int &bottom_end) {
	DEBUG("SUBPOLYGON: " << top_start << ", " << top_end << ", " << bottom_start << ", " << bottom_end);
	int num_vertices = _verts.size();
	std::vector<int> subpolygon;

	for(int i = top_start; i != top_end; i = constrain(i + 1, num_vertices)) {
		subpolygon.push_back(i);
	}
	subpolygon.push_back(top_end);

	for(int i = bottom_start; i != bottom_end; i = constrain(i + 1, num_vertices)) {
		subpolygon.push_back(i);
	}
	subpolygon.push_back(bottom_end);

	top_start = top_end;
	bottom_end = bottom_start;

	return subpolygon;
}

std::vector<std::vector<int>> Polygon::partition() {
	int num_vertices = _verts.size();

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

	auto ltr_compare = [&] (int &lhs, int &rhs) -> bool { return _verts[lhs][0] > _verts[rhs][0]; };
	std::priority_queue<int, std::vector<int>, decltype(ltr_compare)> ltr_order(ltr_compare);

	// Get leftmost and rightmost vertices and order vertices from left-to-right in ltr_order
	int left_index = 0;
	int right_index = 0;
	ltr_order.push(0);
	for(int i = 1; i < num_vertices; ++i) {
		ltr_order.push(i);
		if(_verts[i][0] < _verts[left_index][0])
			left_index = i;
		else if(_verts[i][0] > _verts[right_index][0])
			right_index = i;
	}

	// Construct linked list of vertices in order of placement in the polygon
	Node *root = new Node(left_index, nullptr, nullptr, nullptr, nullptr);
	Node *rightmost = nullptr;
	Node *current = root;
	for(int i = left_index + 1; i < num_vertices + left_index; ++i) {
		Node *child = new Node(i % num_vertices, current, nullptr, nullptr, nullptr);
		current->next = child;
		current = child;

		if(child->index == right_index)
			rightmost = child;
	}
	current->next = root;
	root->prev = current;

	// Add links to left and right vertices in the linked list
	Node *next = root;
	for(int i = 0; i < num_vertices; ++i) {
		current = next;
		for(int j = next->index; j < ltr_order.top(); ++j)
			next = next->next;
		for(int j = next->index; j > ltr_order.top(); --j)
			next = next->prev;

		current->right = next;
		next->left = current;
		ltr_order.pop();
	}

#ifdef DEBUG_MODE
	Node *node = root;
	std::string ltr_str = "";
	for(int i = 0; i < num_vertices; ++i) {
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
	for(int i = 0; i < num_vertices; ++i) {
		int index = current->index;

		if(_verts[index][0] > _verts[left_index][0] && _verts[index][0] < _verts[constrain(index - 1, num_vertices)][0] && _verts[index][0] < _verts[constrain(index + 1, num_vertices)][0]) { // Split
			partitions.push_back(get_subpolygon(start_index, std::min(index, current->left->index), std::max(index, current->left->index), close_index));
		} else if(_verts[index][0] < _verts[right_index][0] && _verts[index][0] > _verts[constrain(index - 1, num_vertices)][0] && _verts[index][0] > _verts[constrain(index + 1, num_vertices)][0]) { // Merge
			partitions.push_back(get_subpolygon(start_index, std::min(index, current->right->index), std::max(index, current->right->index), close_index));
		}

		if(current == rightmost)
			break;
		else
			current = current->right;
	}

	partitions.push_back(get_subpolygon(start_index, right_index, constrain(right_index + 1, num_vertices), close_index));

#ifdef DEBUG_MODE
	DEBUG("Created " << partitions.size() << " partitions:");
	for(auto partition : partitions) {
		for(int i = 0; i < partition.size(); ++i) {
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
	for(int i = 0; i < num_vertices; ++i) {
		vertex_string += std::to_string(indices[i]) + " ";
	}
	DEBUG("Triangulating " + vertex_string);
#endif

	int left_index = 0; // Index of the leftmost point (start point)
	int right_index = 0; // Index of the rightmost point (end point)

	for(int i = 0; i < num_vertices; ++i) {
		// Find leftmost and rightmost vertices
		if(_verts[indices[i]][0] < _verts[indices[left_index]][0])
			left_index = i;
		else if(_verts[indices[i]][0] > _verts[indices[right_index]][0])
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
		if((_verts[indices[constrain(a + 1, num_vertices)]][0] < _verts[indices[constrain(b - 1, num_vertices)]][0] || b == right_index) && a != right_index) {
			a = constrain(a + 1, num_vertices);
			last = current;
			current = a;
		} else {
			b = constrain(b - 1, num_vertices);
			last = current;
			current = b;
		}
		if(last != left_index) remaining_vertices.push_back(last);

		/* If the next vertex is on the bottom half of the polygon and the previous
		 * vertices that don't form triangles are on the top half (or vice-versa),
		 * then the current point and all of the previous points will form a series
		 * of triangles shadderd similarly to a chinese fan. If a fan of triangles is
		 * formed, add each of the triangles in the fan
		 */
		DEBUG(indices[a] << ", " << indices[b] << ", " << indices[left_index] << " | " << indices_index << ", " << _num_elmns);
		if(indices_index < _num_elmns && a != left_index && b != left_index) {
			if((current == a && current - 1 != last) || (current == b && current + 1 != last)) {
				DEBUG("Fan");
#ifdef DEBUG_MODE
				std::string str = "";
				for(auto i : remaining_vertices) str += std::to_string(indices[i]) + " ";
				DEBUG("Remaining vertices: " << str);
#endif

				int num_remaining_vertices = remaining_vertices.size();
				for(int j = 0; j < num_remaining_vertices - 1; ++j) {
					// Add vertices in clockwise order
					int vert_a = remaining_vertices.front();
					remaining_vertices.erase(remaining_vertices.begin());
					int vert_b = remaining_vertices.front();
					if(current == vert_a || current == vert_b) {
						DEBUG("ABORTING FAN: " << indices[current] << ", " << indices[vert_a] << ", " << indices[vert_b]);
						break;
					}

					_gl_indices[indices_index++] = indices[current];
					if(_verts[indices[vert_a]][1] < _verts[indices[vert_b]][1]) {
						_gl_indices[indices_index++] = indices[vert_a];
						_gl_indices[indices_index++] = indices[vert_b];
					} else {
						_gl_indices[indices_index++] = indices[vert_b];
						_gl_indices[indices_index++] = indices[vert_a];
					}

					DEBUG("Resultant indices: " << _gl_indices[indices_index - 3] << ", " << _gl_indices[indices_index - 2] << ", " << _gl_indices[indices_index - 1]);
				}
#ifdef DEBUG_MODE
				str = "";
				for(auto i : remaining_vertices) str += std::to_string(indices[i]) + " ";
				DEBUG("Remaining vertices: " << str);

				std::cout << std::endl;
#endif
				} else { // If a fan is not formed, check if a triangular ear is formed
				if(std::find(remaining_vertices.begin(), remaining_vertices.end(), constrain(current - 2, num_vertices)) != remaining_vertices.end()) {
					double old_slope = double(_verts[indices[constrain(current - 2, num_vertices)]][1] - _verts[indices[constrain(current - 1, num_vertices)]][1]) /
						double(_verts[indices[constrain(current - 2, num_vertices)]][0] - _verts[indices[constrain(current - 1, num_vertices)]][0]);
					double new_slope = double(_verts[indices[constrain(current - 2, num_vertices)]][1] - _verts[indices[constrain(current, num_vertices)]][1]) /
						double(_verts[indices[constrain(current - 2, num_vertices)]][0] - _verts[indices[constrain(current, num_vertices)]][0]);

					if(current == a && new_slope < old_slope) { // Ear on top
						DEBUG("Upper ear around " << indices[current]);
						_gl_indices[indices_index++] = indices[current];
						_gl_indices[indices_index + 2] = indices[remaining_vertices.back()];
						remaining_vertices.pop_back();
						_gl_indices[indices_index++] = indices[remaining_vertices.back()];
						++indices_index;
						DEBUG("Resultant indices: " << _gl_indices[indices_index - 3] << ", " << _gl_indices[indices_index - 2] << ", " << _gl_indices[indices_index - 1] << std::endl);
					} else if(current == b && new_slope > old_slope) { // Ear on bottom
						DEBUG("Lower ear around " << indices[current]);
						_gl_indices[indices_index++] = indices[current];
						_gl_indices[indices_index++] = indices[remaining_vertices.back()];
						remaining_vertices.pop_back();
						_gl_indices[indices_index++] = indices[remaining_vertices.back()];
						DEBUG("Resultant indices: " << _gl_indices[indices_index - 3] << ", " << _gl_indices[indices_index - 2] << ", " << _gl_indices[indices_index - 1] << std::endl);
					}
				}
			}
			}
		}

#ifdef DEBUG_MODE
	std::string indices_str = "";
	for(int i = 0; i < (num_vertices - 2) * 3; ++i) {
		indices_str += "(";
		indices_str += std::to_string(_gl_indices[start_index + i]) + ", ";
		indices_str += std::to_string(_gl_indices[start_index + (++i)]) + ", ";
		indices_str += std::to_string(_gl_indices[start_index + (++i)]);
		indices_str += ") ";
	}
	DEBUG("Partition indices: " << indices_str << std::endl);
#endif

	start_index += num_vertices;
	}

void Polygon::gen_gl_data() {
	if(!_cache_status.gl_data) {
		_cache_status.gl_data = true;

		int num_vertices = _verts.size();
		_num_elmns = (num_vertices - 2) * 3;

		if(_gl_verts) delete[] _gl_verts;
		_gl_verts = new GLfloat[num_vertices * 3];
		for(int i = 0; i < num_vertices; ++i) {
			// Format vertices for OpenGL
			_gl_verts[(i)* 3] = _verts[i][0];
			_gl_verts[(i)* 3 + 1] = _verts[i][1];
			_gl_verts[(i)* 3 + 2] = 0.0f;
		}

		if(_gl_indices) delete[] _gl_indices;
		_gl_indices = new GLuint[(num_vertices - 2) * 3];
		int indices_index = 0; // Index of gl_indices to add to
		int index = 0;

		std::vector<std::vector<int>> partitions = partition();
		for(std::vector<int> indices : partitions) {
			triangulate(indices, index, indices_index);
		}

		_verts_size = sizeof(GLfloat) * num_vertices * 3;
		_indices_size = sizeof(GLint) * (num_vertices - 2) * 3;

#ifdef DEBUG_MODE
		std::string indices_str = "";
		for(int i = 0; i < (num_vertices - 2) * 3; ++i) {
			indices_str += "(";
			indices_str += std::to_string(_gl_indices[i]) + ", ";
			indices_str += std::to_string(_gl_indices[++i]) + ", ";
			indices_str += std::to_string(_gl_indices[++i]);
			indices_str += ") ";
		}
		DEBUG("Indices: " << indices_str << std::endl);
#endif
	}
}

} // adder

std::string to_string(const std::vector<glm::vec3> &vertices) {
	std::string str = "";
	for(const glm::vec3 &vertex : vertices)
		str += "(" + std::to_string(vertex[0]) + ", " + std::to_string(vertex[1]) + ") ";
	return str;
}
