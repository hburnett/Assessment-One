//
//
//
//
//

#ifndef	PARTICLE_H
#define PARTICLE_H

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <FlyCamera.h>
#include <TweakBar.h>

//---------------	GPU PARTICLES	---------------------------------

struct GPUParticle 
{
	GPUParticle() : lifetime(1), lifespan(0) {}

	glm::vec3 position;
	glm::vec3 velocity;

	float lifetime;
	float lifespan;
};

class GPUParticleEmitter
{
public:
	
	GPUParticleEmitter( char* updateShaderFileName );
	virtual ~GPUParticleEmitter();

	unsigned int LoadShader(unsigned int type, const char* path);

	void UpdateTweakBarVariables();

	void SetTweakBarToCurrent();

	void LoadTexture(char * filename);

	void Initialise( unsigned int maxParticles, 
					 float minLife, float maxLife, 
					 float minVel, float maxVel,
					 float startSize, float endSize, 
					 const glm::vec4& startCol,	const glm::vec4& endCol, const glm::vec4& midCol, char * texFilename, TweakBar *tweaks);

	void Draw(float time, const glm::mat4& a_cameraTransform, const glm::mat4& a_projectionView);

	void ChangeUpdateShader(char* updateShaderFilename);

protected:

	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();

	GPUParticle* m_particles;
	char * m_updateShaderFilename;

	unsigned int m_maxParticles;

	glm::vec3 m_position;

	float m_lifespanMin;
	float m_lifespanMax;

	float m_velocityMin;
	float m_velocityMax;

	float m_startSize;
	float m_endSize;

	glm::vec4 m_startColour;
	glm::vec4 m_midColour;
	glm::vec4 m_endColour;

	unsigned int m_activeBuffer;

	unsigned int m_VAO[2];
	unsigned int m_VBO[2];

	unsigned int m_drawShader;
	unsigned int m_updateShader;

	float m_lastDrawTime;
	unsigned int m_texture;
	
	TweakBar *m_tweaks;
};



//-----------	CPU PARTICLES	--------------------------------------------

struct Particle
{
	glm::vec3	position;
	glm::vec3	velocity;
	glm::vec4	colour;
	float		size;
	float		lifetime;
	float		lifespan;

};

struct ParticleVertex
{	
	glm::vec4	position;
	glm::vec4	colour;
};

class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	void Initialise( TweakBar *tweaks, unsigned int maxParticles, unsigned int emitRate,
					 float minLife, float maxLife, 
					 float minVel, float maxVel,
					 float startSize, float endSize, 
					 const glm::vec4& startCol,	const glm::vec4& endCol);

	void Emit(float dT);

	void Update(float dT, FlyCamera* camera);

	void Draw();

protected:

	Particle*		m_particles;
	unsigned int	m_firstDead;
	unsigned int	m_maxParticles;

	glm::vec3 m_position;

	float m_emitTimer;
	float m_emitRate;

	float m_lifespanMin;
	float m_lifespanMax;

	float m_velocityMin;
	float m_velocityMax;

	float m_startSize;
	float m_endSize;

	glm::vec4 m_startColour;
	glm::vec4 m_endColour;
	

	unsigned int	m_VAO, m_VBO, m_IBO;
	ParticleVertex*	m_vertexData;

	TweakBar *m_tweaks;


};
#endif