//
//
//
//
//

#ifndef	PROCEDURALTERRAIN_H
#define PROCEDURALTERRAIN_H

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <FlyCamera.h>
#include <Grid.h>
#include <string>

struct ProceduralVertex 
{
	glm::vec4 position;
	glm::vec2 texCoords;
};
class Perlin
{
public:

	Perlin(unsigned int dimensions, unsigned int program);

	~Perlin();

	unsigned int LoadShader(unsigned int type, const char* path);
	
	void Draw(FlyCamera *m_camera);

	void GenerateGrid();
	
private:
	
	unsigned int m_VAO, m_VBO, m_IBO, m_programID;
	unsigned int m_vertices, m_indexCount;
	unsigned int m_dimensions;


	GLuint m_texture, m_perlinTexture;
};


#endif