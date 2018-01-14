#include "DLA.h"

void randseed() {
	time_t timer;
	time(&timer);
	srand(timer);
}

float randnormal() {
	return (float)rand() / RAND_MAX;
}

BrownianParticle::BrownianParticle(sf::Vector2u size)
{
	this->setPosition(randnormal() * size.x, randnormal() * size.y);
}

BrownianParticle::BrownianParticle()
{
}

BrownianParticle::~BrownianParticle()
{

}

void BrownianParticle::RandMove()
{	
	if (isFixed)
		return;
	sf::Vector2f shift;
	float phi, r = 1.;

	phi = 2. * M_PI * randnormal();
	shift.x = r * cos(phi);
	shift.y = r * sin(phi);
	this->move(shift);
}

float BrownianParticle::DistanceToParticel(BrownianParticle particle)
{
	sf::Vector2f dist = this->getPosition() - particle.getPosition();
	return sqrt(dist.x * dist.x + dist.y * dist.y);
}

bool BrownianParticle::Reaction(BrownianParticle particle)
{
	if (DistanceToParticel(particle) <= reactDist)
	{
		isFixed = true;
	}
	return isFixed;
}

