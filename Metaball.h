#ifndef _METABALL_
#define _METABALL_

#include <vector>
#include "vec.h"
#include "modelerdraw.h"
#include "MarchingCubes.h"

class Metaball
{
public:
	
	struct Ball {
		Vec3d pos;
		double size;
	};

	Metaball();
	void init();
	void draw();
	Vec3d IndexToPos(int i, int j, int k);
	void calcGrid();
	void addBall(Vec3d pos, double size);

	double domain_X0 = -5.0;
	double domain_X1 = 5.0;
	double domain_Y0 = -5.0;
	double domain_Y1 = 5.0;
	double domain_Z0 = -5.0;
	double domain_Z1 = 5.0;

	double*** grid = NULL;
	int gridSize;
	double step;
	std::vector<Ball> balls;
	int quality;

};

#endif
