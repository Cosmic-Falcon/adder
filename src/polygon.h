#ifndef GEO_POLYGON_H
#define GEO_POLYGON_H

#include <algorithm>
#include <array>
#include <queue>
#include <vector>

#include "gfx.h"
#include "krypton_constants.h"

namespace krypt {
	using Vertices = std::vector<glm::vec4>;

	class Polygon {
	public:
		Polygon(std::vector<glm::vec4> vertices, glm::vec4 pos = glm::vec4{0.f, 0.f, 0.f, 1.f});
		~Polygon();

		void rotate(float ang, const glm::vec4 &axis);
		void set_position(const glm::vec4 &pos);
		void translate(const glm::vec4 &xy);

		glm::vec4 position();
		std::vector<glm::vec4> vertices();

		bool is_convex();
		GLfloat* get_gl_vertices();
		GLuint* get_gl_indices();
		int get_gl_vertices_size();
		int get_gl_indices_size();

		int get_num_elements();
		glm::vec4 get_pos();
	private:
		std::vector<int> get_subpolygon(int &top_start, int top_end, int bottom_start, int &bottom_end);
		std::vector<std::vector<int>> partition();
		void triangulate(std::vector<int> &indices, int &start_index, int &indices_index);
		void gen_gl_data();

		glm::vec4 _pos;
		std::vector<glm::vec4> _verts;

		// Cache data
		struct CacheStatus {
			// True if the cache is up to date
			bool gl_data = false;
			bool is_convex = false;
		};
		CacheStatus _cache_status;

		GLfloat* _gl_verts;
		GLuint* _gl_indices;
		int _verts_size; // Size of _gl_verts in bytes
		int _indices_size; // Size of _gl_indices in bytes
		int _num_elmns; // Number of elements
		bool _is_convex;
	};
}

std::string to_string(const std::vector<glm::vec4> &vertices);

#endif // GEO_POLYGON_H
