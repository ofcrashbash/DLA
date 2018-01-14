#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib> 
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>


class BrownianParticle : public sf::CircleShape 
{

public:
	BrownianParticle();
	BrownianParticle(sf::Vector2u size);
	~BrownianParticle();
	void RandMove();
	bool Reaction(BrownianParticle particle);

private:
	bool isFixed = false;
	float reactDist = 20;
	float DistanceToParticel(BrownianParticle particle);

};

void randseed();

