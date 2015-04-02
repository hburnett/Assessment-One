#include <FlyCamera.h>
#include <glm\ext.hpp>
#include <iostream>
#include <Gizmos.h>


void FlyCamera::Update( float dT )
{
	HandleKeyboardInput(dT);
	HandleMouseInput(dT);
	
	Gizmos::draw(GetProjectionView());

	
}

void FlyCamera::HandleKeyboardInput(float dT)
{

	glm::mat4 trans = GetWorldTransform();
	glm::vec3 vRight = trans[0].xyz;
	glm::vec3 vUp = trans[1].xyz;
	glm::vec3 vForward = trans[2].xyz;
	glm::vec3 moveDir(0.0f);

	moveDir = glm::vec3(0,0,0);

	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDir -= vForward;
	}

	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDir += vForward;
	}

	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDir -= vRight;
	}

	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDir += vRight;
	}

	if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
	}

	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);
	}

	if (glfwGetKey(m_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
	{
		SetFlySpeed( GetFlySpeed() - 1.f);
		std::cout << GetFlySpeed() << std::endl;
	}

	if (glfwGetKey(m_window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
	{
		SetFlySpeed( GetFlySpeed() + 1.f);
		std::cout << GetFlySpeed() << std::endl;
	}

	float fLength = glm::length(moveDir);
	if (fLength > 0.01f && glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		moveDir = ((float)dT * m_flySpeed) * glm::normalize(moveDir);
		SetPosition(GetPosition() + moveDir);
	}

}

void FlyCamera::HandleMouseInput(float dT)
{
	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (m_viewButtonClicked == false)
		{
			int width, height;
			glfwGetFramebufferSize(m_window, &width, &height);

			mX = width / 2.0;
			mY = height / 2.0;

			glfwSetCursorPos(m_window, width / 2, height / 2);

			m_viewButtonClicked = true;
		}

		else
		{
			float springiness = 50.f;
			double d = 1-exp(log(0.5)*springiness*dT);
			double mouseX, mouseY;
			glfwGetCursorPos(m_window, &mouseX, &mouseY);

			double xOffset = (mouseX - mX) * d;
			double yOffset = (mouseY - mY) * d;

			CalculateRot(dT, xOffset, yOffset);

		}

		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);
		glfwSetCursorPos(m_window, width / 2, height / 2);

		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}


	else
	{
		m_viewButtonClicked = false;
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}


void FlyCamera::CalculateRot(float dT, double xOffset, double yOffset)
{
	if (xOffset != 0.0)
	{
		glm::mat4 rot = glm::rotate((float)(m_rotSpeed * dT * -xOffset), glm::vec3(0, 1, 0));
		
		SetWorldTransform(GetWorldTransform() * rot);
	}

	if (yOffset != 0.0)
	{
		glm::mat4 rot = glm::rotate((float)(m_rotSpeed * dT * -yOffset), glm::vec3(1, 0, 0));

		SetWorldTransform(GetWorldTransform() * rot);
	}

	//Clean up rotation
	glm::mat4 oldTrans = GetWorldTransform();
	
	glm::mat4 trans;
	
	glm::vec3 worldUp = glm::vec3(0, 1, 0);
	
	//Right
	trans[0] = glm::normalize( glm::vec4(glm::cross(worldUp, oldTrans[2].xyz()), 0));
	//Up
	trans[1] = glm::normalize(glm::vec4(glm::cross(oldTrans[2].xyz(), trans[0].xyz()), 0));
	//Forward
	trans[2] = glm::normalize(oldTrans[2]);
	
	//Position
	trans[3] = oldTrans[3];

	SetWorldTransform(trans);
}
