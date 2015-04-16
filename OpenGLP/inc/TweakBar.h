


#ifndef	TWEAK_BAR_H
#define TWEAK_BAR_H

#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <AntTweakBar.h>


class TweakBar
{
public:	

	TweakBar( GLFWwindow *window );

	~TweakBar();

	static void OnMouseButton(GLFWwindow*, int b, int a, int m) 
	{
		TwEventMouseButtonGLFW(b, a);
	}

	static void OnMousePosition(GLFWwindow*, double x, double y) 
	{
		TwEventMousePosGLFW((int)x, (int)y);
	}

	static void OnMouseScroll(GLFWwindow*, double x, double y) 
	{
		TwEventMouseWheelGLFW((int)y);
	}

	static void OnKey(GLFWwindow*, int k, int s, int a, int m) 
	{
		TwEventKeyGLFW(k, a);
	}

	static void OnChar(GLFWwindow*, unsigned int c) 
	{
		TwEventCharGLFW(c, GLFW_PRESS);
	}

	static void OnWindowResize(GLFWwindow*, int w, int h) 
	{
		TwWindowSize(w, h);
		glViewport(0, 0, w, h);
	}

	void Draw();

	void SetSeed(unsigned int s);
	
	glm::vec4 m_clearColour;
	glm::vec3 m_lightPos;
	glm::vec3 m_lightColour;
	float m_specPower;

	unsigned int m_seed;
	unsigned int m_amplitude;
	unsigned int m_octaves;


private:

	GLFWwindow *m_window;

	TwBar *m_bar;


};
#endif