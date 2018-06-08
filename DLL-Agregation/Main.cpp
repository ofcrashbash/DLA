#define _USE_MATH_DEFINES

#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <cmath>
#include "DLA.h"

//TODO improve perfomance by providing array

void Update(sf::Vertex &vertex, Particle &particle)
{
	vertex.position.x = particle.x;
	vertex.position.y = particle.y;
	if(particle.isStucked)
		vertex.color = sf::Color::White;
	else 
		vertex.color = sf::Color::Red;
}


//Drawing from thread
void GraphicalThread(sf::RenderWindow *window, DLASimulation *DLAObj)
{	
	std::cout << "Render Thread" << std::endl;

	const int n = DLAObj->ParticleNum;
	sf::VertexArray points(sf::Points, n);

	while (window->isOpen())
	{
		window->clear(sf::Color::Black);
		//draw..
		for (int i = 0; i < n; i++)
			Update(points[i], DLAObj->particles[i]);

		window->draw(points);

		//end of the current frame
		window->display();
	}
}


void DLASimulationThread(DLASimulation const & refDLAObj)
{
	std::cout << "DLA thread" << std::endl;

	DLASimulation & DLAObj = const_cast<DLASimulation &>(refDLAObj);//wtf construction
	while (!DLAObj.particles.empty())
		DLAObj.Simulate();
}


int main(int argc, char * argv[]) 
{
	//window size
	float r = 2.;
	int width = 500 * r, height = 500 * r, N = 10000;

	//DLA simulation intitalization
	DLASimulation DLAObj((float)width, (float)height, N, r);
	//Window initialization
	sf::RenderWindow window(sf::VideoMode(width, height), "Diffuse Limited Agregation");


	//main window initialization

	window.setFramerateLimit(30);
	window.setVerticalSyncEnabled(true);
	window.setActive(false);
	
	//render thread initialzaization
	sf::Thread thread(std::bind(&GraphicalThread, &window, &DLAObj));
	thread.launch();

	//DLA simulation thread starting
	std::thread DLAThread(DLASimulationThread, std::ref(DLAObj));

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

	return 0;
}