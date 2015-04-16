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
#include <TweakBar.h>

struct ProceduralVertex 
{
	glm::vec4 position;
	glm::vec2 texCoords;
	glm::vec4 normal;
};
class Perlin
{
public:

	Perlin(unsigned int dimensions, unsigned int program, float amplitude, float persistence);

	~Perlin();

	GLuint LoadTexture(char * filename);

	void GeneratePerlinNoise( int octaves, float scale, float amplitude, float persistence);

	unsigned int LoadShader(unsigned int type, const char* path);
	
	void Draw(FlyCamera *m_camera);

	void ReloadShader(unsigned int program)		{ m_programID = program; }

	void SetTweakBar(TweakBar * tweaks);	

	void GenerateGrid();

	unsigned int m_dimensions;
	float m_amplitude, m_persistence, m_octaves;
	
	unsigned int m_seed;

private:
	
	unsigned int m_VAO, m_VBO, m_IBO, m_programID;
	unsigned int m_vertices, m_indexCount;

	unsigned int* auiIndices;
	ProceduralVertex* aoVertices;

	GLuint m_texture, m_perlinTexture;
	GLuint m_water, m_sand, m_grass, m_snow;

	TweakBar * m_tweaks;

};


#endif