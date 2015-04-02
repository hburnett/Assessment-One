/*-------------------------------------------------------------------------------
Author: Heath Burnett

Description: Application class that creates a window and holds individual states
-------------------------------------------------------------------------------*/

#ifndef APPLICATION_H
#define	APPLICATION_H
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <Solar.h>
#include <Shader.h>

class Application
{
public:

	Application();

	~Application();

	void Run();

private:

	void Update( float dT );

	void Draw();
	
	GLFWwindow *m_window;

	Solar *m_solarSystem;

	Shader *m_shader;

	float dT;
};
#endif