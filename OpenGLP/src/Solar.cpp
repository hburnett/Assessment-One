#include <Solar.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

Solar::Solar( GLFWwindow *window )
{
	for (int i = 0; i < 8; i++)
		m_solarRotation[i] = 0;
	
	m_window = window;
	m_flyCamera = new FlyCamera();
	m_flyCamera->SetInputWindow(window);
	m_flyCamera->SetLookAt(vec3(0,0,30), vec3(0), vec3(0,1,0));
	m_flyCamera->SetPerspective( 3.14159f / 4.0f, 16.f/9.f, 0.1f, 1500.f ); 
}

Solar::~Solar()
{
	delete m_flyCamera;
}


void Solar::Update( float dT )
{	
	m_solarRotation[0] += 0.05f / 1.0f;
	m_solarRotation[1] += 0.05f / 2.557f;
	m_solarRotation[2] += 0.05f / 4.1477f;
	m_solarRotation[3] += 0.05f / 7.8068f;
	m_solarRotation[4] += 0.05f / 49.227f;
	m_solarRotation[5] += 0.05f / 122.2727f;
	m_solarRotation[6] += 0.05f / 348.69f;
	m_solarRotation[7] += 0.05f / 684.02555f;
	
}

void Solar::Draw( float dT )
{
	// Sun
	Gizmos::addSphere(vec3(0), 10.f, 40, 40, vec4(1, 0.7f, 0, 1));
	// Mercury
	Gizmos::addSphere(vec3(20 * sin(m_solarRotation[0]), 0, 20 * cos(m_solarRotation[0])), 0.07f, 5, 5, vec4(0.25f, 0.25f, 0.25f, 1));
	// Venus
	Gizmos::addSphere(vec3(25 * sin(m_solarRotation[1]), 0, 25 * cos(m_solarRotation[1])), .1f, 5, 5, vec4(0.8f, 0.6f, 0, 1));
	// Earth
	Gizmos::addSphere(vec3(33 * sin(m_solarRotation[2]), 0, 33 * cos(m_solarRotation[2])), .11f, 5, 5, vec4(0, 0.7f, 0, 1));
	// Mars
	Gizmos::addSphere(vec3(38 * sin(m_solarRotation[3]), 0, 38 * cos(m_solarRotation[3])), 0.08f, 5, 5, vec4(1, 0.2f, 0, 1));
	// Jupiter		
	Gizmos::addSphere(vec3(50 * sin(m_solarRotation[4]), 0, 50 * cos(m_solarRotation[4])), .8f, 10, 10, vec4(0.5f, 0.25f, 0.25f, 1));
	// Saturn
	Gizmos::addSphere(vec3(70 * sin(m_solarRotation[5]), 0, 70 * cos(m_solarRotation[5])), .75f, 10, 10, vec4(0.5f, 0.25f, 0.25f, 1));
	// Uranus
	Gizmos::addSphere(vec3(85 * sin(m_solarRotation[6]), 0, 85 * cos(m_solarRotation[6])), .75f, 10, 10, vec4(0, 0.25f, 1, 1));
	// Neptune
	Gizmos::addSphere(vec3(100 * sin(m_solarRotation[7]), 0, 100 * cos(m_solarRotation[7])), .75f, 10, 10, vec4(0, 0.0f, 1, 1));
	
	Gizmos::addTransform(glm::mat4(1));
	vec4 white(1);
	vec4 red(1, 0, 0, 1);
	vec4 black(0,0,0,1);
	for (int i = 0; i < 21; ++i) 
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i), i == 10 ? red : black);
	}

	
	m_flyCamera->Update(dT);
}