#include <BaseCamera.h>
#include <glm/ext.hpp>


BaseCamera::BaseCamera()
{
	m_worldTransform = glm::mat4(1);	
	m_cameraDirection = glm::vec3(0);
}

BaseCamera::BaseCamera( glm::vec3 pos )
{
	m_worldTransform = glm::mat4(1);
	m_worldTransform[3] = glm::vec4(pos, 1);	
	m_cameraDirection = glm::vec3(0);
}

BaseCamera::BaseCamera( glm::mat4 transform )
{
	m_worldTransform = transform;	
	m_cameraDirection = glm::vec3(0);
}

void BaseCamera::SetPerspective( float fieldOfView, float aspectRatio, float fNear, float fFar )
{
	m_projectionTransform = glm::perspective(fieldOfView, aspectRatio, fNear, fFar);
	UpdateProjectionViewTransform();
}

void BaseCamera::SetLookAt( glm::vec3 pos, glm::vec3 to, glm::vec3 up )
{
	m_worldTransform = glm::inverse(glm::lookAt(pos, to, up));
	UpdateProjectionViewTransform();
}

void BaseCamera::SetPosition( glm::vec3 position )
{
	m_worldTransform[3] = glm::vec4(position, 1);
}

void BaseCamera::SetWorldTransform( glm::mat4 transform )
{
	m_worldTransform = transform;
	UpdateProjectionViewTransform();
}

void BaseCamera::UpdateProjectionViewTransform()
{
	m_viewTransform = glm::inverse(m_worldTransform);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}

void BaseCamera::Update( float dt )
{
	
} 

glm::mat4 BaseCamera::GetProjectionView() 
{ 
	return m_projectionViewTransform; 
}




