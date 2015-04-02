//
//
//
//
//

#ifndef	GRID_H
#define GRID_H

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <FlyCamera.h>
#include <TweakBar.h>
#include <string>

struct Vertex
{
	glm::vec4 position;
	glm::vec4 colour;
};

class Grid
{
public:

	Grid(TweakBar *tweaks, unsigned int m_programID);

	~Grid();

	unsigned int LoadShader(unsigned int type, const char* path);

	void GenerateGrid(); 

	void Draw(FlyCamera *camera);


private:

	unsigned int m_VAO, m_VBO, m_IBO, m_programID;
	unsigned int m_vertices, m_indexCount;

	TweakBar *m_tweaks;

	GLuint m_texture;
};


#endif