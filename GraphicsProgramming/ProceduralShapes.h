#pragma once

#include "glut.h"
#include <gl/gl.h>
#include <gl/glu.h>

class ProceduralShapes
{
public:
	//segments means vertices per axis
	void GenerateHill(int segments, float size, float height);
	void GenerateWaves(int segments,float size, float waveMagnitude, float waveFrequency, float waveTimescale, int time);

};