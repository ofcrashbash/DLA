#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <stdlib.h>             /* srand, rand */
#include <time.h>       /* time */
#include <vector>
#include <iostream>


sf::Vector2f RandomWalkShift() {
	sf::Vector2f coord;
	if (rand() % 2 == 1)
		coord.x = -1;
	else
		coord.x = 1;
	if (rand() % 2 == 1)
		coord.y = -1;
	else
		coord.y = 1;

	return coord;
}

sf::CircleShape GetCircle() {
	sf::CircleShape circle;
	sf::Color fill_color = sf::Color(255, 0, 0, 200);
	sf::Color outline_color = sf::Color(255, 255, 255, 255);
	circle.setFillColor(fill_color);
	circle.setRadius(10);
	circle.setPosition(400, 300);
	circle.setOutlineColor(outline_color);
	circle.setOutlineThickness(2);

	return circle;
}

int main(int argc, char *argv[]) {

	//window initialization
	sf::RenderWindow window(sf::VideoMode(800, 600), "My lalala");
	window.setFramerateLimit(30);
	window.setVerticalSyncEnabled(true);
	

	//input instaces
	sf::Keyboard keyboard;
	sf::Mouse mouse;

	std::vector<sf::CircleShape> circle_free;
	std::vector<sf::CircleShape> circle_stucked;

	sf::CircleShape circle = GetCircle();

	while (window.isOpen()) 
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					std::cout << "closed" << std::endl;
					window.close();
					break;
				case sf::Event::KeyPressed:
					std::cout << "the escape key was pressed" << std::endl;
					std::cout << "control:" << event.key.control << std::endl;
					std::cout << "alt:" << event.key.alt << std::endl;
					std::cout << "shift:" << event.key.shift << std::endl;
					std::cout << "system:" << event.key.system << std::endl;
					std::cout << "key pressed - " << event.key.code << std::endl;
					break;
				case sf::Event::KeyReleased:
					std::cout << "key released - " << event.key.code << std::endl;
					break;
				case sf::Event::LostFocus:
					std::cout << "lost focus" << std::endl;
					window.requestFocus();
					break;
				case sf::Event::GainedFocus:
					std::cout << "gained focus" << std::endl;
					break;
				case sf::Event::TextEntered:
					std::cout << " text entered " << event.text.unicode << std::endl;
					break;
				case sf::Event::Resized:
					std::cout << "resized" << std::endl;
					break;
				case sf::Event::MouseMoved:
					std::cout << " coords -  " << event.mouseMove.x <<
						", " << event.mouseMove.y << std::endl;
				case sf::Event::MouseButtonPressed:
					std::cout << " button - " << event.mouseButton.button << std::endl;
					break;

				default:
					std::cout << "some default event occured" << std::endl;
					break;
			}
		}

		window.clear(sf::Color::Black);

		//place to update
		circle.setPosition(circle.getPosition() + RandomWalkShift());

		//keyboard state explained
		if (keyboard.isKeyPressed(keyboard.Left))
		{
			std::cout << " left key pressed" << std::endl;
		}

		if (mouse.isButtonPressed(mouse.Left))
		{
			std::cout << " left mouse pressed " << std::endl;
		}

		window.draw(circle);

		window.display();
	}
	
	return 0;
}