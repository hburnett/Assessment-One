/*-------------------------------------------------------------------
Author: Heath Burnett

Description: Contains everything need to create a 3d object/mesh
-------------------------------------------------------------------*/

#ifndef OBJECT_H
#define OBJECT_H

#include <FlyCamera.h>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include <TweakBar.h>

struct TexturedVertex
{
	glm::vec4 position;
	glm::vec4 normal;
	glm::vec2 texCoord;
};

struct BaseVertex
{
	glm::vec4 position;
	//glm::vec4 colour;
	glm::vec4 normal;
};

class Object
{
public:

	// Use this and pass in the base program ID
	// If the mesh has no texture
	Object(unsigned int programID, float xOffset, float zOffset, TweakBar* tweaks, char * objectFilename);

	// Use this contructor if the mesh
	// Has an accompanying texture
	Object(unsigned int programID, float xOffset, float zOffset, TweakBar* tweaks, char * objectFilename, char * textureName);

	~Object();
	
	void Update( float dT );

	void Draw( float dT, FlyCamera *camera );
	
private:

	void LoadObjectFile();
	void LoadTexture(char * filename);


	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_indexCount;
	unsigned int m_vertexCount;
	unsigned int m_programID;
	unsigned int m_texture;

	float m_xOffset;
	float m_zOffset;
	
	glm::vec3 m_lightPos;

	char* m_objectFilename;
	char* m_textureFilename;
	bool m_hasTexture;
	bool m_hasNormal;
	bool m_isQuad;

	
	std::vector<TexturedVertex> m_texturedVertexVector;
	std::vector<BaseVertex> m_baseVertexVector;

	std::vector<glm::vec4> m_vertexPosition;
	std::vector<glm::vec2> m_vertexTexCoordVector;
	std::vector<glm::vec4> m_vertexNormalVector;

	std::vector<unsigned int> m_faceVector;
	std::vector<unsigned int> m_faceTexCoordVector;
	std::vector<unsigned int> m_faceNormalVector;

	TweakBar *m_tweaks;

};
#endif