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

void Update(sf::Vertex &vertex, Particle &particle, sf::Color color)
{
	vertex.position.x = particle.x;
	vertex.position.y = particle.y;
	vertex.color      = color;
}


//Drawing from thread
void GraphicalThread(sf::RenderWindow *window, DLASimulation *DLAObj)
{	
	std::cout << "Render Thread" << std::endl;

	sf::VertexArray points(sf::Points, DLAObj->ParticleNum);

	while (window->isOpen())
	{
		window->clear(sf::Color::Black);
		//draw..
		mtx.lock();
		auto n = DLAObj->free_particles.size();
		for (unsigned int i = 0; i < n; ++i)
			Update(points[i], DLAObj->free_particles[i], sf::Color::White);

		auto m = DLAObj->stucked_particles.size();
		for (auto i = n; i < m + n; ++i)
			Update(points[i], DLAObj->stucked_particles[i - n], sf::Color::Red);
		mtx.unlock();

		window->draw(points);

		//end of the current frame
		window->display();
	}
}


void DLASimulationThread(DLASimulation const & refDLAObj)
{
	std::cout << "DLA thread" << std::endl;

	DLASimulation & DLAObj = const_cast<DLASimulation &>(refDLAObj);//TODO wtf construction
	while (!DLAObj.free_particles.empty())
	{
		mtx.lock();
		DLAObj.Simulate();
		mtx.unlock();
	}
}


int main(int argc, char * argv[]) 
{
	//window size
	float r = 20.;
	int width = 50 * r, height = 50 * r, N = 100;

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
	//TODO why i used one staandard thread and one from sfml?????
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