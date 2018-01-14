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
		DLASimulation(float w, float h, int N = 10)
		{
			ParticleNum = N;
			width = w;
			height = h;

			//random generator initialization
			time_t timer;
			time(&timer);
			srand(timer);

			//free particles
			for (int i = 0; i < N; i++) 
			{
				Particle particle;
				SetRandomPosition(particle);
				particles.push_back(particle);
			}

			//and one stucked particle
			int stuckedParticleIndex = 0;
			particles[stuckedParticleIndex].isStucked = true;
			particles[stuckedParticleIndex].x = w / 2;
			particles[stuckedParticleIndex].x = h / 2;
		}


		~DLASimulation()
		{
			//there is no dynamic memory allocation
		}


		void Simulate()
		{
			//O(n^2) solution at first
			std::vector<Particle> colidedParticles;
			for (auto &particle : particles)
			{
				if (CheckColisions(particle))
				{
					colidedParticles.push_back(particle);
					particle.isStucked = true;
				}
				else 
				{
					RandMove(particle);
					PeriodicalBoundaries(particle);
				}
			}
		}


		void RandMove(Particle &p)
		{
			float phi = 2. * M_PI * (float)RandNormal();
			p.Move(ds * cos(phi), ds * sin(phi));
		}

		

		std::vector<Particle> particles;
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

		void PeriodicalBoundaries(Particle &p)
		{
			if (p.x < 0)
				p.x += width;
			else if (p.x >= width)
				p.x -= width;

			if (p.y < 0)
				p.y += height;
			else if (p.y >= height)
				p.y -= height;
		}
};

void Update(sf::Vertex &vertex, Particle &particle)
{
	vertex.position.x = particle.x;
	vertex.position.y = particle.y;
	vertex.color = sf::Color::Yellow;
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
	int width = 1000, height = 700, N = 10000;

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

	DLAThread.join();

	return 0;
}