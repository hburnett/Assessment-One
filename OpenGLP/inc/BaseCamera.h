/*-----------------------------------------------------
Author: Heath Burnett

Description: Base camera class, used with Gizmos and GML.
//-----------------------------------------------------*/


#ifndef BASE_CAMERA_H
#define	BASE_CAMERA_H

#include <glm/glm.hpp>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>

class BaseCamera
{
public:	
	BaseCamera();
	BaseCamera( glm::vec3 pos );
	BaseCamera( glm::mat4 transform );

	virtual void Update( float dT );

	void SetPerspective( float fieldOfView, float aspectRatio, float fNear, float fFar );
	void SetLookAt( glm::vec3 pos, glm::vec3 to, glm::vec3 up );
	
	void SetPosition( glm::vec3 position );
	glm::vec3 GetPosition() { return m_worldTransform[3].xyz(); }

	void SetWorldTransform( glm::mat4 transform );
	glm::mat4 GetWorldTransform() { return m_worldTransform; }

	glm::mat4 GetView() { return m_viewTransform; }
	glm::mat4 GetProjection() { return m_projectionTransform; }
	glm::mat4 GetProjectionView();

	void SetInputWindow(GLFWwindow *window) { m_window = window; }

protected:

	void UpdateProjectionViewTransform();

	glm::mat4 m_worldTransform;
	glm::mat4 m_viewTransform;
	glm::mat4 m_projectionTransform;
	glm::mat4 m_projectionViewTransform;
	
	glm::mat4 xRot;
	glm::mat4 yRot;
	
	glm::vec3 m_cameraDirection;
	glm::vec3 m_upChange;
	glm::vec3 m_rightChange;
	glm::vec3 m_forwardChange;

	GLFWwindow *m_window;
};
#endif