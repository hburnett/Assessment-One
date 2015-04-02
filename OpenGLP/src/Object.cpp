#include <Object.h>
#include <stb_image.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <cstdlib>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Object::Object(unsigned int programID, float xOffset, float zOffset, TweakBar* tweaks, char * objectFilename)
{
	m_programID = programID;
	m_objectFilename = objectFilename;

	m_indexCount  = 0;
	m_vertexCount = 0;
	m_hasTexture = false;

	m_xOffset = xOffset;
	m_zOffset = zOffset;

	m_tweaks = tweaks;
	
	m_lightPos = vec3(0, 0, 0);
	
	LoadObjectFile();
}

Object::Object(unsigned int programID, float xOffset, float zOffset, TweakBar* tweaks, char * objectFilename, char * textureName)
{
	m_programID = programID;
	m_objectFilename = objectFilename;
	m_textureFilename = textureName;

	m_indexCount  = 0;
	m_vertexCount = 0;
	m_hasTexture = true;

	m_xOffset = xOffset;
	m_zOffset = zOffset;

	m_tweaks = tweaks;
	
	m_lightPos = vec3(0, 0, 0);
	
	LoadObjectFile();
}

void Object::LoadObjectFile()
{
	if (m_hasTexture)
	{
		#pragma region HAS TEXTURE
		
		std::string filenameString(m_objectFilename);
		filenameString = filenameString[filenameString.length() - 5];

		char objType[4];

		strcpy_s(objType, filenameString.c_str());

		std::ifstream in(m_objectFilename, std::ios::in);
		if (!in) 
		{ 
			std::cout << "Cannot open " << m_objectFilename << std::endl; exit(1);
		}
 
		std::string line;
		while (getline(in, line))
		{
			if (line.substr(0,2) == "v ")
			{
				std::istringstream s(line.substr(2));
				TexturedVertex a;
				vec4 vertexPosition; s >> vertexPosition.x; s >> vertexPosition.y; s >> vertexPosition.z; vertexPosition.w = 1.0f;
				vertexPosition.x += m_xOffset;
				vertexPosition.z += m_zOffset;
				a.position = vertexPosition;

				m_texturedVertexVector.push_back(a);
			}  

			else if (line.substr(0,2) == "vt")
			{
				std::istringstream s(line.substr(2));
				vec2 vertexTexCoord; s >> vertexTexCoord.x; s >> vertexTexCoord.y;
				vertexTexCoord.y *= -1.0f;
				m_vertexTexCoordVector.push_back(vertexTexCoord);
			}
			
			else if (line.substr(0,2) == "vn")
			{
				std::istringstream s(line.substr(2));
				vec4 vertexNormal; s >> vertexNormal.x; s >> vertexNormal.y; s >> vertexNormal.z;
				vertexNormal.w = 1;
				//vertexNormal.y *= -1.0f;
				m_vertexNormalVector.push_back(vertexNormal);
			}  

			else if (line.substr(0,2) == "f ") 
			{
				// Vertex, Normal, Texture
				unsigned int v1, v2, v3, v4;
				unsigned int n1, n2, n3, n4;
				unsigned int t1, t2, t3, t4;

				static bool firstFace = true;
				int bestFit = 0;

				#pragma region FIRST FACE

				// Used to determine structure of faces in the obj file
				if (firstFace)
				{					
					int structure1 = sscanf_s(line.c_str(), "f %d %d %d", &v1, &v2, &v3);
					int structure2 = sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
					int structure3 = sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
					int structure4 = sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
					int structure5 = sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4);

					if (structure1 > bestFit)
					{
						bestFit = structure1;
						m_hasTexture = false;
						m_hasNormal = false;
						m_isQuad = false;
					}
					
					if (structure2 > bestFit)
					{
						bestFit = structure2;
						m_hasTexture = true;
						m_hasNormal = false;
						m_isQuad = false;
					}

					if (structure3 > bestFit)
					{
						bestFit = structure3;
						m_hasTexture = false;
						m_hasNormal = true;
						m_isQuad = false;
					}

					if (structure4 > bestFit)
					{
						bestFit = structure4;
						m_hasTexture = true;
						m_hasNormal = true;
						m_isQuad = false;
					}

					if (structure5 > bestFit)
					{
						bestFit = structure5;
						m_hasTexture = true;
						m_hasNormal = true;
						m_isQuad = true;
					}

					firstFace = false;
				}

				#pragma endregion

				// Structure 1
				//"f  %d  %d  %d"
				// Only vertices
				#pragma region STRUCTURE
				if ( !m_hasTexture && !m_hasNormal && !m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d %d %d", &v1, &v2, &v3);
					
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);
				}
#pragma endregion

				// Structure 2
				//"f  %d/%d  %d/%d  %d/%d"
				// Vertices and Texture
				#pragma region STRUCTURE
				if ( m_hasTexture && !m_hasNormal && !m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
					
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);
									
					m_texturedVertexVector[v1 - 1].texCoord = m_vertexTexCoordVector[t1 - 1];
					m_texturedVertexVector[v2 - 1].texCoord = m_vertexTexCoordVector[t2 - 1];
					m_texturedVertexVector[v3 - 1].texCoord = m_vertexTexCoordVector[t3 - 1];
				}
				#pragma endregion

				// Structure 3
				//"f  %d//%d  %d//%d  %d//%d"
				// Vertices and Normal
				#pragma region STRUCTURE
				if ( !m_hasTexture && m_hasNormal && !m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
					
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);									
				}
				#pragma endregion

				// Structure 4
				//"f  %d/%d/%d  %d/%d/%d  %d/%d/%d  %d/%d/%d"
				// Vertices, Texture, Normal
				#pragma region STRUCTURE
				if ( m_hasTexture && m_hasNormal && !m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
										
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);
									
					m_faceTexCoordVector.push_back(t1 - 1); 
					m_faceTexCoordVector.push_back(t2 - 1);
					m_faceTexCoordVector.push_back(t3 - 1);

					m_texturedVertexVector[v1 - 1].texCoord = m_vertexTexCoordVector[t1 - 1];
					m_texturedVertexVector[v2 - 1].texCoord = m_vertexTexCoordVector[t2 - 1];
					m_texturedVertexVector[v3 - 1].texCoord = m_vertexTexCoordVector[t3 - 1];
					
					m_texturedVertexVector[v1 - 1].normal = m_vertexNormalVector[n1 - 1];
					m_texturedVertexVector[v2 - 1].normal = m_vertexNormalVector[n2 - 1];
					m_texturedVertexVector[v3 - 1].normal = m_vertexNormalVector[n3 - 1];	
				}
				#pragma endregion

				// Structure 5
				//"f  %d/%d/%d  %d/%d/%d  %d/%d/%d  %d/%d/%d"
				// Vertices, Texture, Normal and Quad
				#pragma region STRUCTURE
				if ( m_hasTexture && m_hasNormal && m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4);
										
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);
					
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v3 - 1);
					m_faceVector.push_back(v4 - 1);
					
					
					m_texturedVertexVector[v1 - 1].texCoord = m_vertexTexCoordVector[t1 - 1];
					m_texturedVertexVector[v2 - 1].texCoord = m_vertexTexCoordVector[t2 - 1];
					m_texturedVertexVector[v3 - 1].texCoord = m_vertexTexCoordVector[t3 - 1];	
					m_texturedVertexVector[v4 - 1].texCoord = m_vertexTexCoordVector[t4 - 1];
				
					m_texturedVertexVector[v1 - 1].normal = m_vertexNormalVector[n1 - 1];
					m_texturedVertexVector[v2 - 1].normal = m_vertexNormalVector[n2 - 1];
					m_texturedVertexVector[v3 - 1].normal = m_vertexNormalVector[n3 - 1];	
					m_texturedVertexVector[v4 - 1].normal = m_vertexNormalVector[n4 - 1];

				}
				#pragma endregion
				
			}
		}

		m_vertexCount = m_texturedVertexVector.size();

		
		

		std::vector<TexturedVertex> test;
		TexturedVertex* aoVertices = new TexturedVertex[ m_vertexCount ];
		for ( unsigned int i = 0; i < m_vertexCount; ++i )
		{
			aoVertices[i].position = m_texturedVertexVector[i].position;
			aoVertices[i].texCoord = m_texturedVertexVector[i].texCoord;
			aoVertices[i].normal = m_texturedVertexVector[i].normal;
			TexturedVertex vert;
			vert.position = m_texturedVertexVector[i].position;
			vert.texCoord = m_texturedVertexVector[i].texCoord;
			vert.normal = m_texturedVertexVector[i].normal;

			if (m_texturedVertexVector[i].texCoord.x <= 0.0f || m_texturedVertexVector[i].texCoord.y >= 0.0f)
			{
				unsigned int a = i;
			}
			test.push_back( vert );
		}
		

		unsigned int* auiIndices = new unsigned int[ m_faceVector.size() ];
		std::vector<unsigned int> test1;

		for ( unsigned int i = 0 ; i < m_faceVector.size(); i++ ) 
		{
			auiIndices[i] = m_faceVector[i];
			m_indexCount += 1;

			test1.push_back(m_faceVector[i]);
		}

		//for (unsigned int i = 0; i < m_faceVector.size(); i+=3) 
		//{
		//	GLushort ia = auiIndices[i];
		//	GLushort ib = auiIndices[i+1];
		//	GLushort ic = auiIndices[i+2];
		//	glm::vec3 normal = glm::normalize(glm::cross(
		//		glm::vec3(aoVertices[ib].position) - glm::vec3(aoVertices[ia].position),
		//		glm::vec3(aoVertices[ic].position) - glm::vec3(aoVertices[ia].position)
		//		));
		//	aoVertices[ia].normal = aoVertices[ib].normal = aoVertices[ic].normal = vec4(normal, 1);
		//}

		
		// Create VAO, VBO and IBO
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_IBO);

		// Bind buffers
		glBindVertexArray( m_VAO );
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

		// send data to the buffers	
		glBufferData(GL_ARRAY_BUFFER, (m_vertexCount) * sizeof(TexturedVertex), aoVertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
		
		// describe how the vertices are setup so they can be sent to the shader
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)(sizeof(vec4)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)(sizeof(vec4)*2));

		// m_VAO hold all our ARRAY_BUFFER and ARRAY_ELEMENT_BUFFER settings
	// so just rebind it later before using glDrawElements
		glBindVertexArray(0);

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


		delete[] aoVertices;
		delete[] auiIndices;
		
		LoadTexture(m_textureFilename);

		#pragma endregion
	}

	else
	{
		#pragma region BASE

		std::string filenameString(m_objectFilename);
		filenameString = filenameString[filenameString.length() - 5];

		char objType[4];

		strcpy_s(objType, filenameString.c_str());

		std::ifstream in(m_objectFilename, std::ios::in);
		if (!in) 
		{ 
			std::cout << "Cannot open " << m_objectFilename << std::endl; exit(1);
		}
 
		std::string line;
		while (getline(in, line))
		{
			if (line.substr(0,2) == "v ")
			{
				std::istringstream s(line.substr(2));
				BaseVertex a;
				vec4 vertexPosition; s >> vertexPosition.x; s >> vertexPosition.y; s >> vertexPosition.z; vertexPosition.w = 1.0f;
				a.position = vertexPosition;;

				m_baseVertexVector.push_back(a);
			}  

			else if (line.substr(0,2) == "vt")
			{
				
			}  

			else if (line.substr(0,2) == "vn")
			{
				std::istringstream s(line.substr(2));
				vec4 vertexNormal; s >> vertexNormal.x; s >> vertexNormal.y; s >> vertexNormal.z;
				vertexNormal.w = 1;
				//vertexNormal.y *= -1.0f;
				m_vertexNormalVector.push_back(vertexNormal);
			}  

			else if (line.substr(0,2) == "f ") 
			{
				// Vertex, Normal, Texture
				unsigned int v1, v2, v3, v4;
				unsigned int n1, n2, n3, n4;
				unsigned int t1, t2, t3, t4;

				static bool firstFace = true;
				int bestFit = 0;

				#pragma region FIRST FACE

				// Used to determine structure of faces in the obj file
				if (firstFace)
				{					
					int structure1 = sscanf_s(line.c_str(), "f %d %d %d", &v1, &v2, &v3);
					int structure2 = sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
					int structure3 = sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
					int structure4 = sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
					int structure5 = sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4);

					if (structure1 > bestFit)
					{
						bestFit = structure1;
						m_hasTexture = false;
						m_hasNormal = false;
						m_isQuad = false;
					}
					
					if (structure2 > bestFit)
					{
						bestFit = structure2;
						m_hasTexture = true;
						m_hasNormal = false;
						m_isQuad = false;
					}

					if (structure3 > bestFit)
					{
						bestFit = structure3;
						m_hasTexture = false;
						m_hasNormal = true;
						m_isQuad = false;
					}

					if (structure4 > bestFit)
					{
						bestFit = structure4;
						m_hasTexture = true;
						m_hasNormal = true;
						m_isQuad = false;
					}

					if (structure5 > bestFit)
					{
						bestFit = structure5;
						m_hasTexture = true;
						m_hasNormal = true;
						m_isQuad = true;
					}

					firstFace = false;
				}

				#pragma endregion

				// Structure 1
				//"f  %d  %d  %d"
				// Only vertices
				#pragma region STRUCTURE
				if ( !m_hasTexture && !m_hasNormal && !m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d %d %d", &v1, &v2, &v3);
					
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);
				}
#pragma endregion

				// Structure 2
				//"f  %d/%d  %d/%d  %d/%d"
				// Vertices and Texture
				#pragma region STRUCTURE
				if ( m_hasTexture && !m_hasNormal && !m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
					
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);
				}
				#pragma endregion

				// Structure 3
				//"f  %d//%d  %d//%d  %d//%d"
				// Vertices and Normal
				#pragma region STRUCTURE
				if ( !m_hasTexture && m_hasNormal && !m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
					
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);									
				}
				#pragma endregion

				// Structure 4
				//"f  %d/%d/%d  %d/%d/%d  %d/%d/%d  %d/%d/%d"
				// Vertices, Texture, Normal
				#pragma region STRUCTURE
				if ( m_hasTexture && m_hasNormal && !m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
										
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);
									
				}
				#pragma endregion

				// Structure 5
				//"f  %d/%d/%d  %d/%d/%d  %d/%d/%d  %d/%d/%d"
				// Vertices, Texture, Normal and Quad
				#pragma region STRUCTURE
				if ( m_hasTexture && m_hasNormal && m_isQuad )
				{
					sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4);
										
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v2 - 1);
					m_faceVector.push_back(v3 - 1);
					
					m_faceVector.push_back(v1 - 1); 
					m_faceVector.push_back(v3 - 1);
					m_faceVector.push_back(v4 - 1);
									
				}
				#pragma endregion
			}
		}

		m_vertexCount = m_baseVertexVector.size();
		
		
		std::vector<BaseVertex> test;
		BaseVertex* aoVertices = new BaseVertex[ m_vertexCount ];
		for ( unsigned int i = 0; i < m_vertexCount; ++i )
		{
			aoVertices[i].position = m_baseVertexVector[i].position;
			aoVertices[i].normal = m_baseVertexVector[i].normal;
			BaseVertex vert;
			vert.position = m_baseVertexVector[i].position;
			vert.normal = m_baseVertexVector[i].normal;
			test.push_back( vert );
		}
		

		unsigned int* auiIndices = new unsigned int[ m_faceVector.size() ];
		std::vector<unsigned int> test1;

		for ( unsigned int i = 0 ; i < m_faceVector.size(); i++ ) 
		{
			auiIndices[i] = m_faceVector[i];
			test1.push_back(m_faceVector[i]);

		}

		for (unsigned int i = 0; i < m_faceVector.size(); i+=3) 
		{
			GLushort ia = auiIndices[i];
			GLushort ib = auiIndices[i+1];
			GLushort ic = auiIndices[i+2];
			glm::vec3 normal = glm::normalize(glm::cross(
				glm::vec3(aoVertices[ib].position) - glm::vec3(aoVertices[ia].position),
				glm::vec3(aoVertices[ic].position) - glm::vec3(aoVertices[ia].position)
				));
			aoVertices[ia].normal = aoVertices[ib].normal = aoVertices[ic].normal = vec4(normal, 1);
		}

		m_indexCount = m_faceVector.size();
		m_vertexCount = m_baseVertexVector.size();
		
		// Create VAO, VBO and IBO
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_IBO);

		// Bind buffers
		glBindVertexArray( m_VAO );
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

		// send data to the buffers	
		glBufferData(GL_ARRAY_BUFFER, (m_vertexCount) * sizeof(BaseVertex), aoVertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
		
		// describe how the vertices are setup so they can be sent to the shader
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BaseVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BaseVertex), (void*)(sizeof(vec4)));
		//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4) * 2));

		// m_VAO hold all our ARRAY_BUFFER and ARRAY_ELEMENT_BUFFER settings
		// so just rebind it later before using glDrawElements
		glBindVertexArray(0);

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


		delete[] aoVertices;
		delete[] auiIndices;


	#pragma endregion
	}	
}

void Object::Draw(float dT, FlyCamera *camera )
{
	
	//static float lightRot = 0;
	//lightRot += 0.02f;
	//m_lightPos = vec3(1200 * sin(lightRot), 5, 1200 * cos(lightRot));
	//Gizmos::addSphere(m_lightPos, 1.f, 40, 40, vec4(1.f, 1.f, 1.f, 1.f));

	if (m_hasTexture)
	{
		glUseProgram(m_programID);
		
		// set texture slot 
		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, m_texture);



		// tell the shader where it is 
			
		unsigned int projectionViewUniform = glGetUniformLocation(m_programID,"ProjectionView");
		unsigned int lightPosUniform = glGetUniformLocation(m_programID, "LightPos");
		unsigned int lightColourUniform = glGetUniformLocation(m_programID, "LightColour");
		unsigned int cameraPosUniform = glGetUniformLocation(m_programID, "CameraPos");
		unsigned int specPow = glGetUniformLocation(m_programID, "SpecPow");

		vec3 camPos = camera->GetPosition();
		vec3 clr = m_tweaks->m_lightColour;
		vec3 pos = m_tweaks->m_lightPos;

		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(camera->GetProjectionView()));
		glUniform3f(lightPosUniform, pos.x, pos.y, pos.z);
		glUniform3f(lightColourUniform, clr.r, clr.g, clr.b);
		glUniform3f(cameraPosUniform, camPos.x, camPos.y, camPos.z);
		glUniform1f(specPow, m_tweaks->m_specPower);

		

		projectionViewUniform = glGetUniformLocation(m_programID, "diffuse"); 
		glUniform1i(projectionViewUniform, 0);
		

		glBindVertexArray(m_VAO);

		glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
			
		glBindVertexArray(0);

	}

	else 
	{
		glUseProgram(m_programID);
		
		unsigned int projectionViewUniform = glGetUniformLocation(m_programID,"ProjectionView");
		unsigned int lightPosUniform = glGetUniformLocation(m_programID, "LightPos");
		unsigned int lightColourUniform = glGetUniformLocation(m_programID, "LightColour");
		unsigned int cameraPosUniform = glGetUniformLocation(m_programID, "CameraPos");
		unsigned int specPow = glGetUniformLocation(m_programID, "SpecPow");

		vec3 camPos = camera->GetPosition();
		vec3 clr = m_tweaks->m_lightColour;
		vec3 pos = m_tweaks->m_lightPos;

		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(camera->GetProjectionView()));
		glUniform3f(lightPosUniform, pos.x, pos.y, pos.z);
		glUniform3f(lightColourUniform, clr.r, clr.g, clr.b);
		glUniform3f(cameraPosUniform, camPos.x, camPos.y, camPos.z);
		glUniform1f(specPow, m_tweaks->m_specPower);
						
		
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);			
		glBindVertexArray(0);
	}

}

void Object::LoadTexture(char * filename)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture); 
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);		
}