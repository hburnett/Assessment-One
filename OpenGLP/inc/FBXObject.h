/*-------------------------------------------------------------------
Author: Heath Burnett

Description: Contains everything need to create a 3d fbx object/mesh
-------------------------------------------------------------------*/

#ifndef FBXOBJECT_H
#define FBXOBJECT_H

#include <FlyCamera.h>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <FBXFile.h>
#include <TweakBar.h>
#include <iostream>


class FBXObject
{
public:
	FBXObject(char *filename, unsigned int program, TweakBar *tweaks, bool animated);

	~FBXObject();

	void CreateOpenGLBuffers();
	void CleanupOpenGLBuffers();
	

	void Update(float dT);

	void Draw(FlyCamera *camera);
	
	FBXFile* m_fbx;
	unsigned int m_programID;
	float m_timer;
	bool m_animated;
	
	TweakBar *m_tweaks;

};
#endif