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
	m_lightPos = glm::vec3(3000);
	m_lightColour = glm::vec3(1);
	m_specPower = 1.f;


	minLife			= 3.0f;
	maxLife			= 3.0f;
	minVel			= 1.0f;
	maxVel			= 4.0f;
	startSize		= 0.1f;
	endSize			= 0.1f;
	startCol		= glm::vec4(1, 1, 0, 1);
	endCol			= glm::vec4(0, 0, 0, 0.25f);
	midCol			= glm::vec4(0.8f, 0, 0, 1);
	rows			= 64;
	cols			= 64;

	TwAddVarRW(m_bar, "Clear colour", TW_TYPE_COLOR4F, &m_clearColour[0], "");
	TwAddVarRW(m_bar, "Rows", TW_TYPE_UINT8, &rows, "");
	TwAddVarRW(m_bar, "Cols", TW_TYPE_UINT8, &cols, "");
	TwAddVarRW(m_bar, "Light Position", TW_TYPE_DIR3F, &m_lightPos[0], "group=Light");
	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_lightColour[0], "group=Light");
	TwAddVarRW(m_bar, "Specular", TW_TYPE_FLOAT, &m_specPower, "group=Light");
	
	TwAddVarRW(m_bar, "Min Life", TW_TYPE_FLOAT, &minLife, "group=Particles");
	TwAddVarRW(m_bar, "Max Life", TW_TYPE_FLOAT, &maxLife, "group=Particles");
	TwAddVarRW(m_bar, "Min Velocity", TW_TYPE_FLOAT, &minVel, "group=Particles");
	TwAddVarRW(m_bar, "Max Velocity", TW_TYPE_FLOAT, &maxVel, "group=Particles");
	TwAddVarRW(m_bar, "Start Size", TW_TYPE_FLOAT, &startSize, "group=Particles");
	TwAddVarRW(m_bar, "End Size", TW_TYPE_FLOAT, &endSize, "group=Particles");	
	TwAddVarRW(m_bar, "Start Colour", TW_TYPE_COLOR4F, &startCol, "group=Particles");
	TwAddVarRW(m_bar, "Mid Colour", TW_TYPE_COLOR4F, &midCol, "group=Particles");
	TwAddVarRW(m_bar, "End Colour", TW_TYPE_COLOR4F, &endCol, "group=Particles");

	
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