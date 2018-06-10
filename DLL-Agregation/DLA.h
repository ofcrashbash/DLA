class Particle;
//TODO create array of stucked particles and free
using particle_map_el = struct {
	bool isStucked = false;
	Particle* particles[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
	unsigned short int part_count = 6;
};
//TODO try to inherit this class from sf::Vertex or other
//!!!!!!! it will resolve problem with mutexes!! 
class Particle {
public:
	float x = 0., y = 0., r = 10., m = 1.;

	Particle()
	{
		int i = 0;
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

	void Move(float* shift)
	{
		x += shift[0];
		y += shift[1];
	}

	void SetPosition(float x_new, float y_new)
	{
		x = x_new;
		y = y_new;
	}

	void SetPosition(float* coords)
	{
		x = coords[0];
		y = coords[1];
	}

	void Stuck()
	{}

	friend std::ostream& operator<<(std::ostream& os, Particle const& particle) 
	{
		os << "coords - " << particle.x << ", " << particle.y << std::endl;

	}


	bool CollisionTrig(Particle &p)
	{
		//TODO should we stuck particle now or not
		return true;
	}

	friend float Distance(Particle &p1, Particle &p2)
	{
		return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
	}
};


class DLASimulation
{
public:

	std::vector<Particle> free_particles;
	std::vector<Particle> stucked_particles;
	int ParticleNum = 10;

	DLASimulation(float w, float h, int N, float r): ParticleNum(N), width(w), height(h), reactionRad(r)
	{

		//array initialization
		//array size:
		n = (int)round(height / reactionRad);
		m = (int)round(width / reactionRad);
		//array of pointers
		particle_map = new particle_map_el* [n];
		for (int i = 0; i < n; ++i)
		{
			particle_map[i] = new particle_map_el[m];
		}

		//reserving memory for particle containers
		free_particles.reserve(ParticleNum);
		stucked_particles.reserve(ParticleNum);

		//random generator initialization
		time_t timer;
		time(&timer);
		srand(timer);

		//free particles initialization
		for (int i = 0; i < N - 1; i++)
		{
			Particle particle;
			float x, y;
			GetRandomPosition(&x, &y);
			particle.SetPosition(x, y);
			free_particles.push_back(particle);
		}
		//and one stucked particle
		int stuckedParticleIndex = 0;
		Particle particle;
		particle.SetPosition(w / 2., h / 2.);
		//checking position of initial stucked particle in particle map
		ChecoutNewParticle(particle);
	}

	~DLASimulation()
	{
		
		for (int i = 0; i < m; ++i)
			delete[] particle_map[i];
		delete[] particle_map;

	}

	void Simulate()
	{
		
		std::vector<int> particles_index_to_remove;

		int i = 0;
		for (auto &particle : free_particles)
		{
			//std::cout << free_particles << std::endl;

			PeriodicalBoundaries(&particle.x, &particle.y);

			if (auto target_particle = CheckColisions(particle))
			{
				
				if (particle.CollisionTrig(*target_particle))
				{
					particle.Stuck();
					ChecoutNewParticle(particle);
					particles_index_to_remove.push_back(i);
				}
				
			}
			else
				particle.Move(RandMove());

			++i;
		}

		//remove stucked particles from free_particle array
		for (auto index : particles_index_to_remove)
			free_particles.erase(free_particles.begin() + index);

	}



private:

	float ds = 1., reactionRad = 5.;
	float width, height;
	particle_map_el **particle_map;
	int n, m;


	float* RandMove()
	{
		float phi = 2. * M_PI * (float)Rand0_1();
		float shift_coords[] = { ds * cos(phi), ds * sin(phi) };
		return shift_coords;
	}
	

	//TODO modify this function by using particle_map
	Particle* CheckColisions(Particle &p)
	{
		int i, j;
		CoordsToIndexes(p.x, p.y, &i, &j);
		if (particle_map[i][j].isStucked)
		{

			auto adjacent_particles = GetAdjacentParticles(i, j);
			for (auto particle : adjacent_particles)
			{
				if (particle != &p && Distance(p, *particle) <= reactionRad)
				{
					return particle;
				}
			}
		}
		return NULL;
	}

	float Rand0_1()
	{
		return (float)rand() / RAND_MAX;
	}

	void GetRandomPosition(float* x, float* y)
	{
		*x = Rand0_1() * width;
		*y = Rand0_1() * height;
	}

	void PeriodicalBoundaries(float* x, float* y)
	{
		float eps = 0.0001;//used to fix patricle_map[i][j] index out bounds
		if (*x < 0.)
			*x += width - eps;
		else if (*x >= width)
			*x -= width;

		if (*y < 0.)
			*y += height - eps;
		else if (*y >= height)
			*y -= height;
	}

	int* CoordsToIndexes(float* coords) 
	{
		int indexes[2];
		indexes[0] = (int)floor(coords[0] / reactionRad);
		indexes[1] = (int)floor(coords[1] / reactionRad);
		return indexes;
	}

	void CoordsToIndexes(float x, float y, int *i, int* j)
	{
		*i = (int)floor(x / reactionRad);
		*j = (int)floor(y / reactionRad);
	}

	void GetLocalCoords(float x, float y, float* x_loc, float* y_loc)
	{
		int i, j;
		CoordsToIndexes(x, y, &i, &j);//Is it correct? cos it is right value isn't it?
		*x_loc = x - i * reactionRad;
		*y_loc = y - j * reactionRad;
	}

	void PrintParticleMap()
	{
		std::cout << "Printing of array" << std::endl;
		for (int i = 0; i < m; ++i)
		{
			for (int j = 0; j < n; ++j)
				std::cout << particle_map[j][i].isStucked << "  ";
			std::cout << std::endl;
		}
	}

	void ChecoutNewParticle(Particle p)
	{
		
		float x_loc, y_loc;
		GetLocalCoords(p.x, p.y, &x_loc, &y_loc);

		//adding stucked particle to stucked_particles array
		stucked_particles.push_back(p);
		//we should now get pointer from new place.. 
		//from stucked_particle vector.. not from free_particles
		auto particle_pointer = &stucked_particles.back();
		
		int di = 0; 
		if (x_loc <= reactionRad / 2.)
			di = -1;
		else 
		{
			di = +1;
			x_loc = reactionRad - x_loc;
		}

		int dj = 0;
		if (y_loc <= reactionRad / 2.)
			dj = -1;
		else
		{
			dj = +1;
			y_loc = reactionRad - y_loc;
		}


		int i, j;
		CoordsToIndexes(p.x, p.y, &i, &j);

		//checking out current cell
		particle_map[i][j].isStucked = true;
		AddPointer(particle_pointer, i, j);

		//checking out left or right cell if it isn't out of bounds
		if (i + di >= 0 && i + di < n)
			particle_map[i + di][j].isStucked = true;

		//checking out top or bottom cell if it isn't out of bounds
		if (j + dj >= 0 && j + dj < n)
			particle_map[i][j + dj].isStucked = true;

		//checking out diagonal cell if the point is close enough
		auto r2 = x_loc * x_loc + y_loc * y_loc;
		if (r2 <= reactionRad * reactionRad / 4 && i + di >= 0 && i + di < n && j + dj >= 0 && j + dj < m)
			particle_map[i + di][j + dj].isStucked = true;
	}

	void AddPointer(Particle *p, int i, int j)
	{
		for (int k = 0; k < particle_map[i][j].part_count; ++k)
			if (particle_map[i][j].particles[k] == NULL)
			{
				particle_map[i][j].particles[k] = p;
				break;
			}
	}

	std::vector<Particle*> GetAdjacentParticles(int i, int j)
	{

		std::vector<Particle*> adjacent_particles;
		for(int di = -1; di <= 1; ++di)
			for(int dj = -1; dj <= 1; ++dj)
				if (i + di >= 0 && i + di < n && j + dj >= 0 && j + dj < m)
				{
					auto count = particle_map[i + di][j + dj].part_count;
					for (int k = 0; k < count; ++k)
					{
						auto particle_pointer = particle_map[i + di][j + dj].particles[k];
						if (particle_pointer != NULL)
							adjacent_particles.push_back(particle_pointer);
					}
						
				}
		return adjacent_particles;
	}
};


