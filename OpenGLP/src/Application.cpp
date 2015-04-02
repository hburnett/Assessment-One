#include <Application.h>


using glm::vec3;
using glm::vec4;
using glm::mat4;

Application::Application()
{
	// Initialises glfw
	if (glfwInit() == false)
		return;

	// Creates m_window
	m_window = glfwCreateWindow(1280, 720, "Open GL", nullptr, nullptr);

	if (m_window == nullptr)
	{
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) 
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return;
	}
	
	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	Gizmos::create();

	// Sets void colour and enables depth
	glClearColor( 0.25f, 0.25f, 1.f, 1.f );
	glEnable(GL_DEPTH_TEST);

	//m_solarSystem = new Solar(m_window);
	m_shader = new Shader(m_window);
}

Application::~Application()
{	
	Gizmos::destroy();
	glfwTerminate();
}

void Application::Run()
{
	float pT = 0;

	while (glfwWindowShouldClose(m_window) == false && glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS) 
	{
		float cT = (float)glfwGetTime();
		dT = cT - pT;
		pT = cT;

		Update(dT);
		Draw();
			
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	return;
}

void Application::Update( float dT )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	m_shader->Update(dT);
	//m_solarSystem->Update(dT);
}

void Application::Draw()
{
	m_shader->Draw(dT);
	//m_solarSystem->Draw(dT);
	//Gizmos::draw(m_solarSystem->m_flyCamera->GetProjection(), m_solarSystem->m_flyCamera->GetView());
}
