/*--------------------------------------------------------------
Author: Heath Burnett

Description: Contains a ratio'd time-to-scale solar system
-------------------------------------------------------------*/

#ifndef SOLAR_H
#define SOLAR_H

#include <FlyCamera.h>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

class Solar
{
public:

	Solar( GLFWwindow *window );

	~Solar();
	
	void Update( float dT );

	void Draw( float dT );
	
private:

	float m_solarRotation[8];
	float m_baseSolarRotation;
	
	GLFWwindow *m_window;
	FlyCamera *m_flyCamera;



};
#endif