#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <thread>

#include <cstdlib>
#include <cmath>


class Particle {
public:
	float x = 0., y = 0., r = 10., m = 1.;
	bool isStucked = false;
	
	Particle() 
	{
	}

	Particle(float xpos, float ypos)
	{
		x = xpos; y = ypos;
	}

	~Particle()
	{
	}

	void Move(float dx, float dy)
	{
		x += dx;
		y += dy;
	}

	void CollisionTrig(Particle &p)
	{

	}

};

class DLASimulation
{
	public:
		DLASimulation(float width, float height, int N = 10)
		{
			//random generator initialization
			time_t timer;
			time(&timer);
			srand(timer);

			ParticleNum = N;
			//free particles
			for (int i = 0; i < N; i++) 
			{

				Particle freeParticle;
				SetRandomPosition(freeParticle);
				freeParts.push_back(freeParticle);
			}

			//and one stucked particle
			Particle stuckedParticle;
			stuckedParticle.isStucked = true;
			stuckedParticle.x = width / 2;
			stuckedParticle.x = height / 2;
			stuckedParts.push_back(stuckedParticle);
		}


		~DLASimulation()
		{
			//there is dynamic memory allocation
		}


		void Simulate()
		{
			//O(n^2) solution at first
			std::vector<Particle> colidedParticles;
			for (auto &freeParticle : freeParts) 
			{
				if (CheckColisions(freeParticle))
				{
					colidedParticles.push_back(freeParticle);
					freeParticle.isStucked = true;
				}
				else 
				{
					RandMove(freeParticle);
				}
			}
		}


		void RandMove(Particle &p)
		{
			float phi = 2. * M_PI * (float)RandNormal();
			p.Move(ds * cos(phi), ds * sin(phi));
		}


		std::vector<Particle> freeParts, stuckedParts;
		int ParticleNum = 10;

	private:

		float ds = 1., reactionRad = 10.;
		float width, height;

		bool CheckColisions(Particle &p)
		{
			return false;
		}

		float RandNormal()
		{
			return (float)rand() / RAND_MAX;
		}

		void SetRandomPosition(Particle& p)
		{
			p.x = RandNormal() * width;
			p.y = RandNormal() * height;
		}
};

sf::CircleShape GetCircle(sf::Vector2u window_size) {
	sf::CircleShape circle;
	sf::Color fill_color = sf::Color(255, 0, 0, 200);
	sf::Color outline_color = sf::Color(255, 255, 255, 255);
	circle.setFillColor(fill_color);
	circle.setRadius(10);
	circle.setOutlineColor(outline_color);
	circle.setOutlineThickness(2);
	return circle;
}

sf::CircleShape ParticleToCircleShape(Particle &p)
{
	sf::CircleShape circle;
	circle.setPosition(p.x, p.y);
	circle.setRadius(p.r);
	circle.setFillColor(sf::Color::Blue);
	circle.setOutlineColor(sf::Color::Cyan);
	return circle;
}

void Update(sf::CircleShape &circle, Particle &particle)
{
	circle.setPosition(particle.x, particle.y);
}

//Drawing from thread
void GraphicalThread(sf::RenderWindow *window, DLASimulation *DLAObj)
{	
	std::cout << "Render Thread" << std::endl;

	const int n = DLAObj->ParticleNum;
	std::vector<sf::CircleShape> circles;
	for (int i = 0; i < n; i++)
		circles.push_back(ParticleToCircleShape(DLAObj->freeParts[i]));

	while (window->isOpen())
	{
		window->clear(sf::Color::Black);
		//draw..
		for (int i = 0; i < n; i++)
		{
			window->draw(circles[i]);
			Update(circles[i], DLAObj->freeParts[i]);
		}

		//end of the current frame
		window->display();
	}
}


void DLASimulationThread(DLASimulation const & refDLAObj)
{
	std::cout << "DLA thread" << std::endl;

	DLASimulation & DLAObj = const_cast<DLASimulation &>(refDLAObj);//wtf construction
	while (!DLAObj.freeParts.empty())
		DLAObj.Simulate();
}


int main(int argc, char * argv[]) 
{
	//window size
	int width = 1000, height = 700, N = 30;

	//DLA simulation intitalization
	DLASimulation DLAObj((float)width, (float)height, N);



	//main window initialization
	sf::RenderWindow window(sf::VideoMode(width, height), "My lalala");
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

	DLAThread.detach();
	return 0;
}