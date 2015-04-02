/*-----------------------------------------------------
Author: Heath Burnett

Description: Base camera class, used with Gizmos and GML.
//-----------------------------------------------------*/


#ifndef FLY_CAMERA_H
#define	FLY_CAMERA_H

#include <BaseCamera.h>
#include <glm/glm.hpp>

class FlyCamera : public BaseCamera
{
public:			

	virtual void Update( float dT );
	
	void SetFlySpeed(float speed) { m_flySpeed = speed; }
	float GetFlySpeed() const { return m_flySpeed; }

	float GetRotSpeed() const { return m_rotSpeed; }
	void SetRotSpeed( float rot ) { m_rotSpeed = rot; }

private:

	void UpdateProjectionViewTransform();

	void HandleKeyboardInput(float dT);
	void HandleMouseInput(float dT);

	float m_flySpeed;
	float m_rotSpeed;
	double mX,mY;
	bool m_viewButtonClicked;

	void CalculateRot(float dT, double xOffset, double yOffset);

};
#endif