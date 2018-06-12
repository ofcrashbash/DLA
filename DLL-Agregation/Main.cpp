#define _USE_MATH_DEFINES

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <cmath>
#include "DLA.h"
#include <mutex>

std::mutex mtx;



//Drawing from thread
void GraphicalThread(DLASimulation const & refDLAObj, sf::RenderWindow const & refWindow)
{	
	std::cout << "Render Thread" << std::endl;

	DLASimulation & DLAObj = const_cast<DLASimulation &>(refDLAObj);//TODO wtf construction
	sf::RenderWindow &window = const_cast<sf::RenderWindow &>(refWindow);

	sf::VertexArray points(sf::Points, DLAObj.ParticleNum);

	while (window.isOpen())
	{
		window.clear(sf::Color::Black);

		//draw..
		window.draw(DLAObj.drawable_particles);

		//end of the current frame
		window.display();
	}
}


void DLASimulationThread(DLASimulation const & refDLAObj, sf::RenderWindow const & refWindow)
{
	std::cout << "DLA thread" << std::endl;

	DLASimulation & DLAObj = const_cast<DLASimulation &>(refDLAObj);//TODO wtf construction
	sf::RenderWindow &window = const_cast<sf::RenderWindow &>(refWindow);
	while (!DLAObj.free_particles.empty() && window.isOpen())
		DLAObj.Simulate();
}


int main(int argc, char * argv[]) 
{
	//window size
	float r = 5.;
	int width = 200 * r, height = 200 * r, N = 5000;

	//DLA simulation intitalization
	DLASimulation DLAObj((float)width, (float)height, N, r);

	//Window initialization
	sf::RenderWindow window(sf::VideoMode(width, height), "Diffuse Limited Agregation");


	//main window initialization
	window.setFramerateLimit(30);
	window.setVerticalSyncEnabled(true);
	window.setActive(false);
	
	//render thread initialzaization
	std::thread GrahicThread(GraphicalThread, std::ref(DLAObj), std::ref(window));

	//DLA simulation thread starting
	std::thread DLAThread(DLASimulationThread, std::ref(DLAObj), std::ref(window));

	//SFLM event loop
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			default:
				break;
			}
		}
	}

	DLAThread.join();
	GrahicThread.join();

	return 0;
}