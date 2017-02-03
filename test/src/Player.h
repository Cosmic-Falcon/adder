#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Graphics.hpp>

#include "../../src/adder.h"


std::vector<sf::VertexArray> verts_to_lines(std::vector<glm::vec2> vertices) {
	std::vector<sf::VertexArray> lines;
	for(int i = 0; i < vertices.size() - 1; ++i) {
		sf::VertexArray line{sf::Lines, 2};
		line[0].position = sf::Vector2f{vertices[i][0], vertices[i][1]};
		line[1].position = sf::Vector2f{vertices[i + 1][0], vertices[i + 1][1]};
		lines.push_back(line);
	}
	sf::VertexArray line{sf::Lines, 2};
	line[0].position = sf::Vector2f{vertices.back()[0], vertices.back()[1]};
	line[1].position = sf::Vector2f{vertices.front()[0], vertices.front()[1]};
	lines.push_back(line);
	return lines;
}
std::vector<sf::VertexArray> verts_to_lines(std::vector<glm::vec4> vertices) {
	std::vector<sf::VertexArray> lines;
	for(int i = 0; i < vertices.size() - 1; ++i) {
		sf::VertexArray line{sf::Lines, 2};
		line[0].position = sf::Vector2f{vertices[i][0], vertices[i][1]};
		line[1].position = sf::Vector2f{vertices[i + 1][0], vertices[i + 1][1]};
		lines.push_back(line);
	}
	sf::VertexArray line{sf::Lines, 2};
	line[0].position = sf::Vector2f{vertices.back()[0], vertices.back()[1]};
	line[1].position = sf::Vector2f{vertices.front()[0], vertices.front()[1]};
	lines.push_back(line);
	return lines;
}

class Player : public adder::Entity, public sf::Drawable {
public:
	Player(glm::vec2 pos = {0, 0}, bool controllable = false) :
		Entity(adder::Body(1, 1, 1, {{0, 0}, { 0, 50 },{25, 75}, { 50, 50 }, {75, 10}, { 50, 0 }}, pos)),
		_ctrlable(controllable) {
	}
	void update(float dt) final {
		if(_ctrlable) {
			float v = .001f;
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				_body.set_velocity(_body.velocity() + glm::vec2{-v, 0});
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				_body.set_velocity(_body.velocity() + glm::vec2{v, 0});
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				_body.set_velocity(_body.velocity() + glm::vec2{0, -v});
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				_body.set_velocity(_body.velocity() + glm::vec2{0, v});
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
				_body.set_velocity({0, 0});
			}
		}
	}
private:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const final {
		for(auto line : verts_to_lines(_body.get_poly().vertices()))
			target.draw(line);
	}
	bool _ctrlable;
};

#endif