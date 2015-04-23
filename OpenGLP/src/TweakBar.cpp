#include <TweakBar.h>




TweakBar::TweakBar( GLFWwindow *window )
{
	m_window = window;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	


	m_bar = TwNewBar("Bar");


	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_clearColour = glm::vec4(0.3f, 0.3f, 0.3f, 1.f);
	m_lightPos = glm::vec3(30000);
	m_lightColour = glm::vec3(1);
	m_specPower = 1.f;

	m_seed = 0;
	m_amplitude = 4;
	m_octaves = 6;

	TwAddVarRW(m_bar, "Clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "Light Position", TW_TYPE_DIR3F, &m_lightPos[0], "group=Light");
	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_lightColour[0], "group=Light");
	TwAddVarRW(m_bar, "Specular", TW_TYPE_FLOAT, &m_specPower, "group=Light");
	
	TwAddVarRW(m_bar, "Amplitude", TW_TYPE_UINT32, &m_amplitude, "group=Procedural");
	TwAddVarRW(m_bar, "Octaves", TW_TYPE_UINT32, &m_octaves, "group=Procedural");
	
}

void TweakBar::Draw()
{
	TwDraw();
}

TweakBar::~TweakBar()
{
	TwDeleteAllBars();
	TwTerminate();
}

void TweakBar::SetSeed(unsigned int s)
{
	m_seed = s;
	TwAddVarRW(m_bar, "Seed", TW_TYPE_UINT32, &m_seed, "group=Procedural");
}
