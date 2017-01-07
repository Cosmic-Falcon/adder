#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "../../src/adder.h"
#include <chrono>

std::vector<sf::VertexArray> verts_to_lines(std::vector<glm::vec4> vertices) {
	std::vector<sf::VertexArray> lines;
	for(int i = 0; i < vertices.size() - 1; ++i) {
		sf::VertexArray line{sf::Lines, 2};
		line[0].position = sf::Vector2f{vertices[i][0], vertices[i][1]};
		line[1].position = sf::Vector2f{vertices[i+1][0], vertices[i+1][1]};
		lines.push_back(line);
	}
	sf::VertexArray line{sf::Lines, 2};
	line[0].position = sf::Vector2f{vertices.back()[0], vertices.back()[1]};
	line[1].position = sf::Vector2f{vertices.front()[0], vertices.front()[1]};
	lines.push_back(line);
	return lines;
}

int main() {
	adder::Body body(1, 1, 1, {{0, 0}, {0, 64}, {64, 64}, {64, 0}}, {320, 320});
	constexpr int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 640;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "ADDER");
	window.setFramerateLimit(60);
	std::chrono::steady_clock::time_point t_0, t_1;
	t_0 = std::chrono::steady_clock::now();
	t_1 = std::chrono::steady_clock::now();

	while(window.isOpen()) {
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
		}
		
		float v = .01f;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			body.set_velocity(body.velocity() + glm::vec2{-v, 0});
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			body.set_velocity(body.velocity() + glm::vec2{v, 0});
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			body.set_velocity(body.velocity() + glm::vec2{0, -v});
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			body.set_velocity(body.velocity() + glm::vec2{0, v});
		}

		t_1 = std::chrono::steady_clock::now();
		float dt = std::chrono::duration_cast<std::chrono::milliseconds>(t_1 - t_0).count();
		body.update(dt);
		t_0 = std::chrono::steady_clock::now();

		window.clear(sf::Color::Black);
		for(auto line : verts_to_lines(body.get_poly().vertices()))
			window.draw(line);
		window.display();

	}
	return 0;
}
