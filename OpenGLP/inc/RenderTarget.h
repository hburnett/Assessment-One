//
//
//
//
//

#ifndef	RENDERTARGET_H
#define RENDERTARGET_H

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <FlyCamera.h>
#include <TweakBar.h>

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Draw(FlyCamera *m_camera);

	void SetupViewport();

private:

	unsigned int m_FBO, m_FBOTexture, m_FBODepth, m_vao, m_vbo, m_ibo, m_programID;

};


#endif