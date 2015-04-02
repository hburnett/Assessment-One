#include <Particle.h>
#include <stb_image.h>


unsigned int GPUParticleEmitter::LoadShader(unsigned int type, const char* path) 
{
	FILE* file = fopen(path, "rb");

	if (file == nullptr)
		return 0;

	// read the shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = new char[length + 1];

	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);

	delete[] source;

	return shader;
}

GPUParticleEmitter::GPUParticleEmitter( char* updateShaderFileName ) :	m_particles(nullptr), m_maxParticles(0),
											m_position(0, 0, 0), m_lastDrawTime(0),
											m_updateShader(0),	m_drawShader(0) 
{
	m_updateShaderFilename = updateShaderFileName;
	m_VAO[0] = 0;
	m_VAO[1] = 0;
	m_VBO[0] = 0;
	m_VBO[1] = 0;
}

GPUParticleEmitter::~GPUParticleEmitter() 
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_VAO);
	glDeleteBuffers(2, m_VBO);

	// delete the shaders
	glDeleteProgram(m_drawShader);
	glDeleteProgram(m_updateShader);
}

void GPUParticleEmitter::Initialise(unsigned int maxParticles, 
									float minLife, float maxLife, 
									float minVel, float maxVel,
									float startSize, float endSize, 
									const glm::vec4& startCol,	const glm::vec4& endCol, const glm::vec4& midCol, char * texFilename, TweakBar *tweaks)
{
	LoadTexture(texFilename);
	m_startColour = startCol;
	m_endColour = endCol;
	m_midColour = midCol;
	m_tweaks = tweaks;
	m_startSize = startSize;
	m_endSize = endSize;
	m_velocityMin = minVel;
	m_velocityMax = maxVel;
	m_lifespanMin = minLife;
	m_lifespanMax = maxLife;
	m_maxParticles = maxParticles;
	
	m_lifespanMin	= m_tweaks->minLife;
	m_lifespanMax	= m_tweaks->maxLife;
	m_velocityMin	= m_tweaks->minVel;
	m_velocityMax	= m_tweaks->maxVel;
	m_startSize		= m_tweaks->startSize;
	m_endSize		= m_tweaks->endSize;
	m_startColour	= m_tweaks->startCol;
	m_endColour		= m_tweaks->endCol;
	m_midColour		= m_tweaks->midCol;

	m_particles = new GPUParticle[maxParticles];

	m_activeBuffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void GPUParticleEmitter::UpdateTweakBarVariables()
{	
	m_lifespanMin	= m_tweaks->minLife;
	m_lifespanMax	= m_tweaks->maxLife;
	m_velocityMin	= m_tweaks->minVel;
	m_velocityMax	= m_tweaks->maxVel;
	m_startSize		= m_tweaks->startSize;
	m_endSize		= m_tweaks->endSize;
	m_startColour	= m_tweaks->startCol;
	m_endColour		= m_tweaks->endCol;
	m_midColour		= m_tweaks->midCol;
}

void GPUParticleEmitter::SetTweakBarToCurrent ()
{
	m_tweaks->minLife	=	m_lifespanMin;	
	m_tweaks->maxLife	=	m_lifespanMax;	
	m_tweaks->minVel	=	m_velocityMin;	
	m_tweaks->maxVel	=	m_velocityMax;	
	m_tweaks->startSize	=	m_startSize;
	m_tweaks->endSize	=	m_endSize;
	m_tweaks->startCol	=	m_startColour;	
	m_tweaks->endCol	=	m_endColour;
	m_tweaks->midCol	=	m_midColour;	
}

void GPUParticleEmitter::CreateBuffers()
{
	// create opengl buffers
	glGenVertexArrays(2, m_VAO);
	glGenBuffers(2, m_VBO);

	// setup the first buffer
	glBindVertexArray(m_VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles *	sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // velocity
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,	sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,	sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,	sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,	sizeof(GPUParticle), ((char*)0) + 28);

	// second buffer
	glBindVertexArray(m_VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles *	sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // velocity
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,	sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,	sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,	sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,	sizeof(GPUParticle), ((char*)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void GPUParticleEmitter::CreateDrawShader()
{
	unsigned int vs = LoadShader(GL_VERTEX_SHADER, "resources/shaders/gpuParticle.vert");
	unsigned int gs = LoadShader(GL_GEOMETRY_SHADER, "resources/shaders/gpuParticleTC.geom");
	unsigned int fs = LoadShader(GL_FRAGMENT_SHADER, "resources/shaders/gpuParticle.frag");

	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, fs);
	glAttachShader(m_drawShader, gs);
	glLinkProgram(m_drawShader);

	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	// bind the shader so that we can set
	// some uniforms that don't change per-frame
	glUseProgram(m_drawShader);

	// bind size information for interpolation that won’t change
	int location = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(location, m_startSize);

	location = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(location, m_endSize);

	// bind colour information for interpolation that wont change
	location = glGetUniformLocation(m_drawShader, "colourStart");
	glUniform4fv(location, 1, glm::value_ptr(m_startColour));

	location = glGetUniformLocation(m_drawShader, "colourEnd");
	glUniform4fv(location, 1, glm::value_ptr(m_endColour));	

	location = glGetUniformLocation(m_drawShader, "colourMid");
	if (location != -1)
		glUniform4fv(location, 1, glm::value_ptr(m_midColour));	


}

void GPUParticleEmitter::LoadTexture(char * filename)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture); 
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);		
}

void GPUParticleEmitter::ChangeUpdateShader(char* updateShaderFilename)
{
	m_updateShaderFilename = updateShaderFilename;
	CreateUpdateShader();
}

void GPUParticleEmitter::CreateUpdateShader()
{
	unsigned int vs = LoadShader(GL_VERTEX_SHADER, m_updateShaderFilename);
	m_updateShader = glCreateProgram();
	glAttachShader(m_updateShader, vs);

	// specify the data that we will stream back
	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_updateShader);

	// remove unneeded handles
	glDeleteShader(vs);

	// bind the shader so that we can set some
	// uniforms that don't change per-frame
	glUseProgram(m_updateShader);

	// bind lifetime minimum and maximum
	int location = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(location, m_lifespanMin);

	location = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(location, m_lifespanMax); 
}

void GPUParticleEmitter::Draw(float time, const glm::mat4& a_cameraTransform, const glm::mat4& a_projectionView) 
{
	// update the particles using transform feedback
	glUseProgram(m_updateShader);

	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// bind time information
	int location = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(location, time);

	location = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(location, m_lifespanMin);

	location = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(location, m_lifespanMax);

	float deltaTime = time - m_lastDrawTime; 
	m_lastDrawTime = time;
	location = glGetUniformLocation(m_updateShader, "deltaTime");
	glUniform1f(location, deltaTime);

	// bind emitter's position
	location = glGetUniformLocation(m_updateShader, "emitterPosition");
	glUniform3fv(location, 1, &m_position[0]);

	glEnable(GL_RASTERIZER_DISCARD);
	
	

	glBindVertexArray(m_VAO[m_activeBuffer]);

	// work out the "other" buffer
	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;

	// bind the buffer we will update into as points
	// and begin transform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_VBO[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	// disable transform feedback and enable rasterization again
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);


	// draw the particles using the Geometry SHader to billboard them
	glUseProgram(m_drawShader);
	
	
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_texture);
	location = glGetUniformLocation(m_drawShader, "diffuse");
	glUniform1i(location, 0);

	// bind size information for interpolation that won’t change
	location = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(location, m_startSize);

	location = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(location, m_endSize);

	// bind colour information for interpolation that wont change
	location = glGetUniformLocation(m_drawShader, "colourStart");
	glUniform4fv(location, 1, glm::value_ptr(m_startColour));

	location = glGetUniformLocation(m_drawShader, "colourEnd");
	glUniform4fv(location, 1, glm::value_ptr(m_endColour));	
		
	location = glGetUniformLocation(m_drawShader, "colourMid");
	if (location != -1)
		glUniform4fv(location, 1, glm::value_ptr(m_midColour));	

	location = glGetUniformLocation(m_drawShader, "projectionView");
	glUniformMatrix4fv(location, 1, false, &a_projectionView[0][0]);

	location = glGetUniformLocation(m_drawShader, "cameraTransform");
	glUniformMatrix4fv(location, 1, false, &a_cameraTransform[0][0]);
	
	// draw particles in the "other" buffer
	glBindVertexArray(m_VAO[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	// swap for next frame
	m_activeBuffer = otherBuffer;

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}







ParticleEmitter::ParticleEmitter() : m_particles(nullptr), m_firstDead(0), m_maxParticles(0), 
									 m_position(0, 0, 0),  m_VAO(0), m_VBO(0), m_IBO(0),
									 m_vertexData(nullptr)
{



}

ParticleEmitter::~ParticleEmitter()
{
	delete[] m_particles;
	delete[] m_vertexData;

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
}

void ParticleEmitter::Initialise( TweakBar *tweaks, unsigned int maxParticles, unsigned int emitRate,
								  float minLife, float maxLife, 
								  float minVel, float maxVel,
								  float startSize, float endSize, 
								  const glm::vec4& startCol, const glm::vec4& endCol)
{
	m_tweaks = tweaks;
	m_maxParticles = maxParticles;

	m_emitTimer = 0;
	m_emitRate = 1.0f / emitRate;

	m_lifespanMin = minLife;
	m_lifespanMax = maxLife;

	m_velocityMin = minVel;
	m_velocityMax = maxVel;

	m_startSize = startSize;
	m_endSize = endSize;

	m_startColour = startCol;
	m_endColour = endCol;

	m_particles = new Particle[m_maxParticles];
	m_firstDead = 0;

	m_vertexData = new ParticleVertex[m_maxParticles * 4];

	unsigned int* indexData = new unsigned int[m_maxParticles * 6];
	
	for (unsigned int i = 0; i < m_maxParticles; ++i) 
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;
		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	// create opengl buffers
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 *	sizeof(ParticleVertex), m_vertexData, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 6 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // colour
	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;
		
}

void ParticleEmitter::Emit(float dT)
{
//	static float timer = 0;
//
//	timer += dT;
//
//	// only emit if there is a dead particle to use
//	if (m_firstDead >= m_maxParticles)
//		return;
//
//	// resurrect the first dead particle
//	Particle& particle = m_particles[m_firstDead++];
//
//	// assign its starting position
//	//particle.position.x = m_position.x + (rand() / (float)RAND_MAX) * m_tweaks->cubeSize.x - m_tweaks->cubeSize.x / 2;
//	//particle.position.y = m_position.y + (rand() / (float)RAND_MAX) * m_tweaks->cubeSize.y - m_tweaks->cubeSize.y / 2;
//	//particle.position.z = m_position.z + (rand() / (float)RAND_MAX) * m_tweaks->cubeSize.z - m_tweaks->cubeSize.z / 2;
//	particle.position = m_position;
//
//	//particle.position.x = m_position.x + (sin(timer) * 15);
//	//particle.position.y = 1;
//	//particle.position.z = m_position.y + (cos(timer) * 15);
//
//
//
//	// randomise its lifespan
//	particle.lifetime = 0;
//	particle.lifespan = (rand() / (float)RAND_MAX) * (m_lifespanMax - m_lifespanMin) + m_lifespanMin;
//
//	// set starting size and colour
//	particle.colour = m_startColour;
//	particle.size = m_startSize;
//
//	// randomise velocity direction and strength
//	float velocity = (rand() / (float)RAND_MAX) *(m_velocityMax - m_velocityMin) + m_velocityMin;
//
//	particle.velocity.x = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f);//0
//	particle.velocity.y = abs((rand()  / (float)RAND_MAX) * 2.0f - 1.0f);
//	particle.velocity.z = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f);//0
//
//
//	
//
//	//particle.velocity.x	= xDif + m_tweaks->firePos.x;
//	//particle.velocity.y	= m_tweaks->firePos.y;
//	//particle.velocity.z	= zDif + m_tweaks->firePos.z;
//	particle.velocity = glm::normalize(particle.velocity) * velocity;


	// only emit if there is a dead particle to use
	if (m_firstDead >= m_maxParticles)
		return;

	// resurrect the first dead particle
	Particle& particle = m_particles[m_firstDead++];

	// assign its starting position
	particle.position = m_position;

	// randomise its lifespan
	particle.lifetime = 0;
	particle.lifespan = (rand() / (float)RAND_MAX) * (m_lifespanMax - m_lifespanMin) + m_lifespanMin;

	// set starting size and colour
	particle.colour = m_startColour;
	particle.size = m_startSize;

	// randomise velocity direction and strength
	float velocity = (rand() / (float)RAND_MAX) * (m_velocityMax - m_velocityMin) + m_velocityMin;

	particle.velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;

	particle.velocity = glm::normalize(particle.velocity) *	velocity;

}

void ParticleEmitter::Update(float dT, FlyCamera* camera)
{
	using glm::vec3;
	using glm::vec4;

	//m_maxParticles	= m_tweaks->maxParticles;
	//m_emitRate		= 1.0f / m_tweaks->emitRate;
	//m_lifespanMin	= m_tweaks->minLife;
	//m_lifespanMax	= m_tweaks->maxLife;
	//m_velocityMin	= m_tweaks->minVel;
	//m_velocityMax	= m_tweaks->maxVel;
	//m_startSize		= m_tweaks->startSize;
	//m_endSize		= m_tweaks->endSize;
	//m_startColour	= m_tweaks->startCol;
	//m_endColour		= m_tweaks->endCol;

	m_emitTimer += dT;
	while (m_emitTimer > m_emitRate)
	{
		Emit(dT);
		m_emitTimer -= m_emitRate;
	}

	unsigned int quad = 0;

	// update particles and turn live particles into billboard quads
	for (unsigned int i = 0; i < m_firstDead; i++) 
	{
	Particle* particle = &m_particles[i];
	particle->lifetime += dT;

		if (particle->lifetime >= particle->lifespan) 
		{
			// swap last alive with this one
			*particle = m_particles[m_firstDead - 1];
			m_firstDead--;
		}

		else
		{			
			// move particle
			particle->position += particle->velocity * dT;

			// size particle
			particle->size = glm::mix(m_startSize, m_endSize, particle->lifetime / particle->lifespan);

			// colour particle
			particle->colour = glm::mix(m_startColour, m_endColour,	particle->lifetime / particle->lifespan);


			// make a quad the correct size and colour
			float halfSize = particle->size * 0.5f;

			m_vertexData[quad * 4 + 0].position = vec4(halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 0].colour	= particle->colour;

			m_vertexData[quad * 4 + 1].position = vec4(-halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 1].colour	= particle->colour;

			m_vertexData[quad * 4 + 2].position = vec4(-halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 2].colour	= particle->colour;

			m_vertexData[quad * 4 + 3].position = vec4(halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 3].colour	= particle->colour;


			// create billboard transform
			vec3 zAxis = glm::normalize(vec3(camera->GetWorldTransform()[3]) - particle->position);
			vec3 xAxis = glm::cross(vec3(camera->GetWorldTransform()[1]), zAxis);
			vec3 yAxis = glm::cross(zAxis, xAxis);
			glm::mat4 billboard(vec4(xAxis, 0), vec4(yAxis, 0), vec4(zAxis, 0), vec4(0,0,0,1));

			
			m_vertexData[quad * 4 + 0].position = billboard * m_vertexData[quad * 4 + 0].position +
			vec4(particle->position,0);

			m_vertexData[quad * 4 + 1].position = billboard * m_vertexData[quad * 4 + 1].position +
			vec4(particle->position,0);

			m_vertexData[quad * 4 + 2].position = billboard * m_vertexData[quad * 4 + 2].position +
			vec4(particle->position,0);

			m_vertexData[quad * 4 + 3].position = billboard * m_vertexData[quad * 4 + 3].position +
			vec4(particle->position,0);

			++quad;
		}
	}

}

void ParticleEmitter::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDead * 4 * sizeof(ParticleVertex), m_vertexData);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_firstDead * 6, GL_UNSIGNED_INT, 0);
}

