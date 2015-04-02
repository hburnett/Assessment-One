/*--------------------------------------------------------------
Author: Heath Burnett

Description: Contains shader tutorial work
-------------------------------------------------------------*/

#ifndef SHADER_H
#define SHADER_H

#include <FlyCamera.h>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include <Object.h>
#include <FBXObject.h>
#include <TweakBar.h>
#include <Particle.h>
#include <RenderTarget.h>
#include <Skybox.h>
#include <Grid.h>
#include <ProceduralTerrain.h>

class Shader
{
public:

	Shader( GLFWwindow *window );

	~Shader();
	
	void Update( float dT );

	void Draw( float dT );

	TweakBar* GetTweakBar() { return m_TweakBar; }

	unsigned int LoadShader(unsigned int type, const char* path);

	void ReloadShaders();
	
private:

	void CreateShaders();
	//void GenerateGrid(unsigned int rows, unsigned int cols);

	//void LoadTexture(char * filename);

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_indexCount;
	unsigned int m_texturedProgramID;
	unsigned int m_animatedProgramID;
	unsigned int m_cpuParticleProgramID;
	unsigned int m_baseProgramID;
	unsigned int m_texture;
	unsigned int m_selectedParticleEmitter;
	unsigned int m_unlitTexturedProgramID;

	GLFWwindow *m_window;
	FlyCamera *m_flyCamera;

	std::vector<Object *> m_untexturedObject;
	std::vector<Object *> m_texturedObject;

	std::vector<FBXObject *> m_fbxUntexturedObjects;
	std::vector<FBXObject *> m_fbxTexturedObjects;

	std::vector<GPUParticleEmitter *> m_gpuParticleEmitters;
	std::vector<RenderTarget *> m_renderTargets;

	std::vector<Skybox *> m_skybox;
	
	std::vector<Grid *> m_grids;
	std::vector<Perlin *> m_terrain;

	TweakBar *m_TweakBar;
};
#endif