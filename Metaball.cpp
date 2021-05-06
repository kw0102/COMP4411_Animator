#include "Metaball.h"

Metaball::Metaball() 
{
	init();
}

void Metaball::init()
{
	ModelerDrawState* mds = ModelerDrawState::Instance();
	quality = mds->m_quality;
	switch (quality)
	{
	case HIGH:
		step = 0.25f; break;
	case MEDIUM:
		step = 0.5f; break;
	case LOW:
		step = 1.0f; break;
	case POOR:
		step = 2.0f; break;
	}

	// deconstruct
	if (grid)
	{
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				delete[] grid[i][j];
			}
			delete[] grid[i];
		}
		delete[] grid;
	}

	gridSize = (domain_X1 - domain_X0) / step;

	// construct
	grid = new double**[gridSize];
	for (int i = 0; i < gridSize; i++) {
		grid[i] = new double* [gridSize];
		for (int j = 0; j < gridSize; j++) {
			grid[i][j] = new double[gridSize];
			for (int k = 0; k < gridSize; k++) {
				grid[i][j][k] = 0.0f;
			}
		}
	}

	calcGrid();

}

Vec3d Metaball::IndexToPos(int i, int j, int k)
{
	return Vec3d(domain_X0 + i * step, domain_Y0 + j * step, domain_Z0 + k * step);
}

void Metaball::draw()
{
	ModelerDrawState* mds = ModelerDrawState::Instance();
	if (quality != mds->m_quality) init();

	double threshold = 1.0f;

	for (int i = 0; i < gridSize-1; i++) {
		for (int j = 0; j < gridSize-1; j++) {
			for (int k = 0; k < gridSize-1; k++) {
				unsigned int index = 0;
				if (grid[i][j][k] > threshold) index |= 1u;
				if (grid[i+1][j][k] > threshold) index |= 2u;
				if (grid[i+1][j+1][k] > threshold) index |= 4u;
				if (grid[i][j+1][k] > threshold) index |= 8u;
				if (grid[i][j][k+1] > threshold) index |= 16u;
				if (grid[i+1][j][k+1] > threshold) index |= 32u;
				if (grid[i+1][j+1][k+1] > threshold) index |= 64u;
				if (grid[i][j+1][k+1] > threshold) index |= 128u;

				for (int n = 0; TRI_TABLE[index][n] != -1; n += 3)
				{
					Vec3d rt = IndexToPos(i, j, k);
					Vec3d v1 = rt + step * EDGE_INDEX[TRI_TABLE[index][n]];
					Vec3d v2 = rt + step * EDGE_INDEX[TRI_TABLE[index][n+1]];
					Vec3d v3 = rt + step * EDGE_INDEX[TRI_TABLE[index][n+2]];

					Vec3d normal = -cross(v1 - v2, v2 - v3);
					if (normal.length() > 0.0001f)
					{
						glBegin(GL_TRIANGLES);
						glNormal3d(normal[0], normal[1], normal[2]);
						glVertex3d(v1[0], v1[1], v1[2]);
						glVertex3d(v2[0], v2[1], v2[2]);
						glVertex3d(v3[0], v3[1], v3[2]);
						glEnd();
					}

				}

			}
		}
	}
}

void Metaball::addBall(Vec3d pos, double size)
{
	balls.push_back(Ball{ pos, size });
	calcGrid();
}

void Metaball::calcGrid()
{

	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			for (int k = 0; k < gridSize; k++) {
				grid[i][j][k] = 0.0f;
			}
		}
	}

	for (int n = 0; n < balls.size(); n++)
	{
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				for (int k = 0; k < gridSize; k++) {
					double d2 = (IndexToPos(i, j, k) - balls[n].pos).length2();
					grid[i][j][k] += balls[n].size * balls[n].size / d2;
				}
			}
		}
	}
}

