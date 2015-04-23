#include <Shader.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <cstdlib>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;


unsigned int Shader::LoadShader(unsigned int type, const char* path) 
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


Shader::Shader( GLFWwindow *window )
{	
	m_window = window;
	m_flyCamera = new FlyCamera();
	m_flyCamera->SetInputWindow(window);
	m_flyCamera->SetLookAt(vec3(0,20,-10), vec3(0), vec3(0,1,0));
	m_flyCamera->SetPerspective( 3.14159f / 4.0f, 16.f/9.f, 0.1f, 4500.f ); 
	m_flyCamera->SetFlySpeed(150.0f);
	m_flyCamera->SetRotSpeed(0.1f);
	m_selectedParticleEmitter = 0;
	
	
	m_TweakBar = new TweakBar(m_window);
	
	CreateShaders();

	m_skybox.push_back(new Skybox("resources/skybox/test/"));
	//m_grids.push_back(new Grid(m_TweakBar, m_baseProgramID));
	//m_grids[0]->GenerateGrid();

	m_terrain.push_back(new Perlin(256, m_unlitTexturedProgramID, 4.0f, 0.3f));
	m_terrain[0]->SetTweakBar(m_TweakBar);
	
	m_gpuParticleEmitters.push_back(new GPUParticleEmitter("resources/shaders/gpuParticleUpdateRandom.vert"));
	m_gpuParticleEmitters[0]->Initialise( 1000000, 1.0f, 5.0f, 20.5f, 50.1f, 0.2f, 0.15f, 
		glm::vec4(1.0f, 0.7f, 0.0f, 1.0f), 	glm::vec4(0.0f, 0.0f, 0.0f, 0.3f),
		glm::vec4(1.0f, 0.2f, 0.0f, 1.0f), "resources/images/pixel.png", m_TweakBar);
	


	m_gpuParticleEmitters.push_back(new GPUParticleEmitter("resources/shaders/gpuParticleUpdateSphere.vert"));
	m_gpuParticleEmitters[1]->Initialise( 1000000, 1.0f, 3.0f, 20.5f, 50.1f, 0.2f, 0.15f, 
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.8f, 1.0f, 1.0f, 0.3f),
		 glm::vec4(0.70f, 1.0f, 1.0f, 1.0f), "resources/images/pixel.png", m_TweakBar);

	//m_renderTargets.push_back(new RenderTarget());

	//m_fbxUntexturedObjects.push_back(new FBXObject("./resources/fbx/models/test.fbx", m_baseProgramID, m_TweakBar, false));
	//m_fbxUntexturedObjects.push_back(new FBXObject("./resources/fbx/models/final.fbx", m_baseProgramID, m_TweakBar, false));
	//m_fbxTexturedObjects.push_back(new FBXObject("./resources/fbx/models/soulspear/soulspear.fbx", m_texturedProgramID, m_TweakBar, false));
	m_fbxTexturedObjects.push_back(new FBXObject("./resources/fbx/models/island/island.fbx", m_texturedProgramID, m_TweakBar, false));
	m_fbxTexturedObjects.push_back(new FBXObject("./resources/fbx/models/totem/totems.fbx", m_texturedProgramID, m_TweakBar, false));
	//m_fbxTexturedObjects.push_back(new FBXObject("./resources/fbx/characters/demolition/demolition.fbx", m_animatedProgramID, m_TweakBar, true));
	//m_fbxTexturedObjects.push_back(new FBXObject("./resources/fbx/characters/enemynormal/enemynormal.fbx", m_animatedProgramID, m_TweakBar, true));


	//m_texturedObject.push_back( new Object(m_texturedProgramID, 0, 0, m_TweakBar, "./resources/fbx/models/MarsPlanet/MarsPlanet.obj", "./resources/fbx/models/MarsPlanet/MarsMap_2500x1250.jpg"));
	//m_texturedObject.push_back( new Object(m_texturedProgramID, "./resources/obj/plane0.obj", "./resources/textures/gun/beretta.png"));
	//m_texturedObject.push_back( new Object(m_texturedProgramID, 0, 0, m_TweakBar, "./resources/obj/mario.obj", "./resources/textures/mario/marioD.jpg"));
	//m_texturedObject.push_back( new Object(m_texturedProgramID, 3, 0, m_TweakBar, "./resources/obj/porsche0.obj", "./resources/textures/car/red.BMP"));
	//m_untexturedObject.push_back( new Object(m_baseProgramID, 0, 0, m_TweakBar, "./resources/obj/islandOBJ.obj"));
	//m_untexturedObject.push_back( new Object(m_baseProgramID, 0, 0, m_TweakBar,"./resources/obj/lounge2.obj"));
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glClearColor(0.3f, 0.3f, 0.3f, 1);
}

Shader::~Shader()
{
	delete m_flyCamera;
}

void Shader::Update( float dT )
{		
	static bool wireframe = false;
	static bool toggleKeyDown = false;
	static bool toggleGDown = false;
	static bool toggleRDown = false;

	for (unsigned int i = 0; i < m_terrain.size(); i++)
	{
		if (m_terrain[i]->m_seed != m_TweakBar->m_seed || m_terrain[i]->m_amplitude != m_TweakBar->m_amplitude || m_terrain[i]->m_octaves != m_TweakBar->m_octaves )
		{			
			m_terrain[i]->m_seed = m_TweakBar->m_seed;
			m_terrain[i]->m_amplitude = m_TweakBar->m_amplitude;
			m_terrain[i]->m_octaves = m_TweakBar->m_octaves;
			float scale = (1.0f / m_terrain[i]->m_dimensions) * 3;
			m_terrain[i]->GeneratePerlinNoise(m_terrain[i]->m_octaves, scale, m_terrain[i]->m_amplitude, m_terrain[i]->m_persistence);
		}
	}


	for (unsigned int i = 0; i < m_fbxUntexturedObjects.size(); i++)
		m_fbxUntexturedObjects[i]->Update(dT);
	
	for (unsigned int i = 0; i < m_fbxTexturedObjects.size(); i++)
		m_fbxTexturedObjects[i]->Update(dT);

	if (glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS && !toggleGDown)
	{
		for (unsigned int i = 0; i < m_terrain.size(); i++)
		{
			float scale = (1.0f / m_terrain[i]->m_dimensions) * 3;
			int octaves = 6;
			m_terrain[i]->GeneratePerlinNoise(octaves, scale, m_terrain[i]->m_amplitude, m_terrain[i]->m_persistence);
		}

		toggleGDown = true;
	}	

	if (glfwGetKey(m_window, GLFW_KEY_G) != GLFW_PRESS)
	{
		toggleGDown = false;
	}

	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS && !toggleRDown)
	{
		ReloadShaders();

		for (unsigned int i = 0; i < m_terrain.size(); i++)
		{
			m_terrain[i]->ReloadShader(m_unlitTexturedProgramID);
		}

		for (unsigned int i = 0; i < m_fbxTexturedObjects.size(); i++)
		{
			m_fbxTexturedObjects[i]->m_programID = (m_texturedProgramID);
		}

		toggleRDown = true;
	}	

	if (glfwGetKey(m_window, GLFW_KEY_R) != GLFW_PRESS)
	{
		toggleRDown = false;
	}
	
	if (glfwGetKey(m_window, GLFW_KEY_T) != GLFW_PRESS)
		toggleKeyDown = false;
	
	if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS && toggleKeyDown == false)
	{		
		toggleKeyDown = true;		
		wireframe = !wireframe;
	}

	if (m_gpuParticleEmitters.size() > 0)
	{
		// SETTING PARTICLE EMITTER
		if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS )
		{
			std::cin >> m_selectedParticleEmitter;
			m_gpuParticleEmitters[m_selectedParticleEmitter]->SetTweakBarToCurrent();
		}

		if (glfwGetKey(m_window, GLFW_KEY_KP_1) == GLFW_PRESS)
		{
			m_gpuParticleEmitters[m_selectedParticleEmitter]->
				ChangeUpdateShader("resources/shaders/gpuParticleUpdate.vert");
		}
		
		if (glfwGetKey(m_window, GLFW_KEY_KP_2) == GLFW_PRESS)
		{
			m_gpuParticleEmitters[m_selectedParticleEmitter]->
				ChangeUpdateShader("resources/shaders/gpuParticleUpdateCube.vert");
		}

		if (glfwGetKey(m_window, GLFW_KEY_KP_3) == GLFW_PRESS)
		{
			m_gpuParticleEmitters[m_selectedParticleEmitter]->
				ChangeUpdateShader("resources/shaders/gpuParticleUpdateSphere.vert");
		}
		
		if (glfwGetKey(m_window, GLFW_KEY_KP_4) == GLFW_PRESS)
		{
			m_gpuParticleEmitters[m_selectedParticleEmitter]->
				ChangeUpdateShader("resources/shaders/gpuParticleUpdateDonut.vert");
		}
		
		if (glfwGetKey(m_window, GLFW_KEY_KP_5) == GLFW_PRESS)
		{
			m_gpuParticleEmitters[m_selectedParticleEmitter]->
				ChangeUpdateShader("resources/shaders/gpuParticleUpdateTower.vert");
		}
		
		if (glfwGetKey(m_window, GLFW_KEY_KP_6) == GLFW_PRESS)
		{
			m_gpuParticleEmitters[m_selectedParticleEmitter]->
				ChangeUpdateShader("resources/shaders/gpuParticleUpdateRandom.vert");
		}
	}
	

	if (wireframe)
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	
	else if (!wireframe)
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}

void Shader::ReloadShaders()
{
	 glDeleteProgram(m_texturedProgramID);
	 glDeleteProgram(m_animatedProgramID);
	 glDeleteProgram(m_cpuParticleProgramID);
	 glDeleteProgram(m_baseProgramID);
	 glDeleteProgram(m_unlitTexturedProgramID);

	 CreateShaders();
}


void Shader::Draw( float dT )
{	
	Gizmos::addTransform(glm::mat4(1));
	
	if(m_gpuParticleEmitters.size() > 0)
		m_gpuParticleEmitters[m_selectedParticleEmitter]->UpdateTweakBarVariables();

	for (unsigned int i = 0; i < m_renderTargets.size(); i++)
		m_renderTargets[i]->SetupViewport();
								
	for (unsigned int i = 0; i < m_skybox.size(); i++)
		m_skybox[i]->Draw(m_flyCamera);		



	for (unsigned int i = 0; i < m_untexturedObject.size(); i++)
		m_untexturedObject[i]->Draw(dT, m_flyCamera);			

	

	for (unsigned int i = 0; i < m_terrain.size(); i++)
		m_terrain[i]->Draw(m_flyCamera);
	
	for (unsigned int i = 0; i < m_texturedObject.size(); i++)
		m_texturedObject[i]->Draw(dT, m_flyCamera);

	
		
	for (unsigned int i = 0; i < m_fbxUntexturedObjects.size(); i++)
		m_fbxUntexturedObjects[i]->Draw(m_flyCamera);
	
	for (unsigned int i = 0; i < m_fbxTexturedObjects.size(); i++)
		m_fbxTexturedObjects[i]->Draw(m_flyCamera);

	for (unsigned int i = 0; i < m_grids.size(); i++)
		m_grids[i]->Draw(m_flyCamera);
			
	
	for( unsigned int i = 0; i < m_gpuParticleEmitters.size(); i++)
	{
		m_gpuParticleEmitters[i]->Draw((float)glfwGetTime(), m_flyCamera->GetWorldTransform(), m_flyCamera->GetProjectionView());
	}
	
		
	//if ( i == 0 )
	//{
	//	for (unsigned int i = 0; i < m_renderTargets.size(); i++)
	//		m_renderTargets[i]->Draw(m_flyCamera);
	//}
	
	m_TweakBar->Draw();
	
	vec4 clr = m_TweakBar->m_clearColour;
	glClearColor( clr.r, clr.g, clr.b, clr.a );

	m_flyCamera->Update(dT);
		
}

void Shader::CreateShaders()
{
// START OF CPU PARTICLE SHADER
		
	int cpuParticleSuccess = GL_FALSE;

	unsigned int vsCP = LoadShader(GL_VERTEX_SHADER, "resources/shaders/cpuParticle.vert");
	unsigned int fsCP = LoadShader(GL_FRAGMENT_SHADER, "resources/shaders/cpuParticle.frag");
	
	m_cpuParticleProgramID = glCreateProgram();
	glAttachShader(m_cpuParticleProgramID, vsCP);
	glAttachShader(m_cpuParticleProgramID, fsCP);
	glLinkProgram(m_cpuParticleProgramID);
	glGetProgramiv(m_cpuParticleProgramID, GL_LINK_STATUS, &cpuParticleSuccess);

	if (cpuParticleSuccess == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_cpuParticleProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_cpuParticleProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader CPU particle program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fsCP);
	glDeleteShader(vsCP);
	

	// END OF CPU PARTICLE SHADER
//-------------------------------------------------------------------------------------------
	// START OF ANIMATED SHADER
	
	int animatedSuccess = GL_FALSE;
	
	unsigned int vsAnim = LoadShader(GL_VERTEX_SHADER, "resources/shaders/animatedFBX.vert");	
	unsigned int fsAnim = LoadShader(GL_FRAGMENT_SHADER, "resources/shaders/animatedFBX.frag");

	m_animatedProgramID = glCreateProgram();
	glAttachShader(m_animatedProgramID, vsAnim);
	glAttachShader(m_animatedProgramID, fsAnim);
	glLinkProgram(m_animatedProgramID);
	glGetProgramiv(m_animatedProgramID, GL_LINK_STATUS, &animatedSuccess);
	
	if (animatedSuccess == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_animatedProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_animatedProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader animation program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}
	
	glDeleteShader(fsAnim);
	glDeleteShader(vsAnim);
	

	// END OF ANIMATED SHADER
//-------------------------------------------------------------------------------------------
	// START OF TEXTURED SHADER
			
	int textureSuccess = GL_FALSE;
	
	unsigned int vsTex = LoadShader(GL_VERTEX_SHADER, "resources/shaders/texturedObject.vert");		
	unsigned int fsTex = LoadShader(GL_FRAGMENT_SHADER, "resources/shaders/texturedObject.frag");
	
	m_texturedProgramID = glCreateProgram();
	glAttachShader(m_texturedProgramID, vsTex);
	glAttachShader(m_texturedProgramID, fsTex);
	glLinkProgram(m_texturedProgramID);
	glGetProgramiv(m_texturedProgramID, GL_LINK_STATUS, &textureSuccess);
	
	if (textureSuccess == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_texturedProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_texturedProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader textured program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}
	
	glDeleteShader(fsTex);
	glDeleteShader(vsTex);
	

	// END OF TEXTURED SHADER
//-------------------------------------------------------------------------------------------
	// START OF UNTEXTURED SHADER
		
	int baseSuccess = GL_FALSE;
	
	unsigned int vsBase = LoadShader(GL_VERTEX_SHADER, "resources/shaders/untexturedObject.vert");		
	unsigned int fsBase = LoadShader(GL_FRAGMENT_SHADER, "resources/shaders/untexturedObject.frag");
		
	m_baseProgramID = glCreateProgram();
	glAttachShader(m_baseProgramID, vsBase);
	glAttachShader(m_baseProgramID, fsBase);
	glLinkProgram(m_baseProgramID);
	glGetProgramiv(m_baseProgramID, GL_LINK_STATUS, &baseSuccess);
	
	if (baseSuccess == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_baseProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_baseProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader base program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}
	 
	glDeleteShader(fsBase);
	glDeleteShader(vsBase);
	
	// END OF UNTEXTURED SHADER
	//-------------------------------------------------------------------------------------------
	// START OF UNLIT TEXTURED SHADER
		
	int unlitSuccess = GL_FALSE;
	
	unsigned int vsUnlit = LoadShader(GL_VERTEX_SHADER, "resources/shaders/texturedUnlitObject.vert");		
	unsigned int fsUnlit = LoadShader(GL_FRAGMENT_SHADER, "resources/shaders/texturedUnlitObject.frag");
		
	m_unlitTexturedProgramID = glCreateProgram();
	glAttachShader(m_unlitTexturedProgramID, vsUnlit);
	glAttachShader(m_unlitTexturedProgramID, fsUnlit);
	glLinkProgram(m_unlitTexturedProgramID);
	glGetProgramiv(m_unlitTexturedProgramID, GL_LINK_STATUS, &unlitSuccess);
	
	if (unlitSuccess == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetShaderiv(m_unlitTexturedProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetShaderInfoLog(m_unlitTexturedProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader unlit program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}
	
	glDeleteShader(fsUnlit);
	glDeleteShader(vsUnlit);
}