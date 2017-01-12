#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "../../src/adder.h"
#include <chrono>
#include "Player.h"

int main() {
	constexpr int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 640;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "ADDER");
	window.setFramerateLimit(60);
	std::chrono::steady_clock::time_point t_0, t_1;
	t_0 = std::chrono::steady_clock::now();
	t_1 = std::chrono::steady_clock::now();
	adder::Manager manager;
	manager.add_entity("p1", new Player({320, 320}));
	manager.add_entity("p2", new Player({260, 270}));

	while(window.isOpen()) {
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
		}
		
		t_1 = std::chrono::steady_clock::now();
		float dt = std::chrono::duration_cast<std::chrono::milliseconds>(t_1 - t_0).count();
		
		manager.update(dt);
		
		t_0 = std::chrono::steady_clock::now();

		window.clear(sf::Color::Black);
		window.draw(*(Player*)(manager.get("p1")));
		window.draw(*(Player*)(manager.get("p2")));

		window.display();
		std::cout << std::endl;
	}
	return 0;
}

