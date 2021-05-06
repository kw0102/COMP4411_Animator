
#include "LSystem.h"

void drawLSysFern(int depth, double angle)
{
	std::string seq = "X";
	
	while (depth > 0)
	{
		std::string newSeq = "";
		for (unsigned int i = 0; i < seq.length(); i++)
		{
			if (seq[i] == 'X') 
			{
				newSeq += "F+[[X]-X]-F[-FX]+X";
			}
			else if (seq[i] == 'F') 
			{
				newSeq += "FF";
			}
			else
			{
				newSeq += seq[i];
			}
		}
		seq = newSeq;
		depth--;
	}

	glPushMatrix();
	setDiffuseColor(0, 1, 0);
	for (unsigned int i = 0; i < seq.length(); i++) 
	{
		switch (seq[i])
		{
		case 'X': break;
		case 'F': drawCylinder(0.1f, 0.1f, 0.1f); glTranslated(0, 0, 0.1f); break;
		case '+': glRotated(angle, 0, 1, 0); break;
		case '-': glRotated(-angle, 0, 1, 0); break;
		case '[': glPushMatrix(); break;
		case ']': glPopMatrix(); break;
		}
	}
	glPopMatrix();
}
