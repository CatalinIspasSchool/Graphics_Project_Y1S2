#include "ProceduralShapes.h"
#include <cmath> 
#include <vector>


void ProceduralShapes::GenerateHill(int segments, float size, float height)
{
	//essentially a quad distorted to have its center rise
	float s = segments;

	for (float i = 0; i + 1 < segments; i++)
	{
		float ip = i + 1;
		for (float j = 0; j + 1 < segments; j++)
		{	
			float jp = j+1;
			/*
				height formula explained: 
			x-x*x gives an arc over 0-1 with a height of 0.25
			divide x (i and j below) by segment count to map the ints to 0-1 based on how far along they are
			add x and z axis together to make a hill
			multiply by 2 to make the results 0-1 then by height to make it 0-height

			there is probably a simpler way to do this (but if it works it works)
			also, everything is a float cause otherwise the calculations get messed up
			*/
			float test = ((i / s - i * i / s / s) + (j / s - j * j / s / s)) * 2 * height;
			glBegin(GL_QUADS);							
				glTexCoord2f(i / s, j / s);
					glVertex3f(i * size / s,		((i / s - i * i / s / s) + (j / s - j * j / s / s)) * 2 * height,			j * size / s);
				glTexCoord2f(ip / s, j / s);
					glVertex3f(ip * size / s,		((ip / s - ip * ip / s / s) + (j / s - j * j / s / s)) * 2 * height,		j * size / s);
				glTexCoord2f(ip / s, jp / s);
					glVertex3f(ip * size / s,		((ip / s - ip * ip / s / s) + (jp / s - jp * jp / s / s)) * 2 * height,		jp * size / s);
				glTexCoord2f(i / s, jp / s);
					glVertex3f(i * size / s,		((i / s - i * i / s / s) + (jp / s - jp * jp / s / s)) * 2 * height,		jp * size / s);
			glEnd();
		}	
	}
}

void ProceduralShapes::GenerateWaves(int segments, float size, float waveMagnitude, float waveFrequency, float waveTimescale, int time)
{
	for (float i = 0; i + 1 < segments; i++)
	{
		float ip = i + 1;
		for (float j = 0; j + 1 < segments; j++)
		{
			float jp = j + 1;

			/*
				height formula explained:
			sin gives the wave its shape, while keeping it bound within the y axis inside the range I want
			x and z are added together to give the wave its direction - diagonal
			time is added inside the sin to give the illusion of motion (the timescale just modifies the rate for this)
			dividing by frequency gives control over how tight the peaks and lows of the sine wave are
			the final result is modified by the magnitude to achieve larger/smaller waves
			*/


			glBegin(GL_QUADS);
			glTexCoord2f(i / segments, j / segments);
				glVertex3f(i * size / segments - size / 2,		sinf((i + j + time * waveTimescale) / waveFrequency) * waveMagnitude,		j * size / segments - size / 2);
			glTexCoord2f(ip / segments, j / segments);
				glVertex3f(ip * size / segments - size / 2,		sinf((ip + j + time * waveTimescale) / waveFrequency) * waveMagnitude,		j * size / segments - size / 2);
			glTexCoord2f(ip / segments, jp / segments);
				glVertex3f(ip * size / segments - size / 2,		sinf((ip + jp + time * waveTimescale) / waveFrequency) * waveMagnitude,		jp * size / segments - size / 2);
			glTexCoord2f(i / segments, jp / segments);
				glVertex3f(i * size / segments - size / 2,		sinf((i + jp + time * waveTimescale) / waveFrequency) * waveMagnitude,		jp * size / segments - size / 2);
			glEnd();
		}
	}
}