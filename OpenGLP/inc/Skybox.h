//
//
//
//
//

#ifndef	SKYBOX_H
#define SKYBOX_H

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <FlyCamera.h>
#include <TweakBar.h>
#include <string>

class Skybox
{
public:

	Skybox(std::string folderDir);

	~Skybox();

	unsigned int LoadShader(unsigned int type, const char* path);

	void LoadTexture(GLenum orientation, const char* filename);

	void Draw(FlyCamera *m_camera);

	void SetupViewport();

private:

	unsigned int m_vao, m_vbo, m_ibo, m_programID;

	GLuint m_texture;
};


#endif