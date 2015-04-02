#include <FBXObject.h>

FBXObject::FBXObject(char * filename, unsigned int program, TweakBar* tweaks, bool animated)
{
	m_programID = program;
	m_tweaks = tweaks;
	m_timer = 0;
	m_animated = animated;

	m_fbx = new FBXFile();
	
	if(!(m_fbx->load(filename)))
	{
		std::cout << filename << " could not be loaded." << std::endl;
	}

	m_fbx->initialiseOpenGLTextures();

	CreateOpenGLBuffers();

}

FBXObject::~FBXObject()
{
	CleanupOpenGLBuffers();
}

void FBXObject::Update(float dT)
{
	if (!m_animated)
	{
		return;
	}

	// grab the skeleton and animation we want to use
	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);

	

	m_timer += dT * 2;

	FBXAnimation* animation = m_fbx->getAnimationByIndex(0);
	// evaluate the animation to update bones
	skeleton->evaluate(animation, m_timer);

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		skeleton->m_nodes[bone_index]->updateGlobalTransform();
	}


}

void FBXObject::Draw(FlyCamera *camera)
{
	glUseProgram(m_programID);

	if(m_animated)
	{
		FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
		skeleton->updateBones();

		int bones_location = glGetUniformLocation(m_programID, "bones");
		glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE,	(float*)skeleton->m_bones);
	}
		
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID,"ProjectionView");
	unsigned int lightPosUniform = glGetUniformLocation(m_programID, "LightPos");
	unsigned int lightColourUniform = glGetUniformLocation(m_programID, "LightColour");
	unsigned int cameraPosUniform = glGetUniformLocation(m_programID, "CameraPos");
	unsigned int specPow = glGetUniformLocation(m_programID, "SpecPow");

	glm::vec3 camPos = camera->GetPosition();
	glm::vec3 clr = m_tweaks->m_lightColour;
	glm::vec3 pos = m_tweaks->m_lightPos;

	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(camera->GetProjectionView()));
	glUniform3f(lightPosUniform, pos.x, pos.y, pos.z);
	glUniform3f(lightColourUniform, clr.r, clr.g, clr.b);
	glUniform3f(cameraPosUniform, camPos.x, camPos.y, camPos.z);
	glUniform1f(specPow, m_tweaks->m_specPower);


	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) 
	{	
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		
		int m_global = glGetUniformLocation(m_programID, "global");
		glUniformMatrix4fv(m_global, 1, GL_FALSE,	&(mesh->m_globalTransform)[0][0]);
			
		if ( mesh->m_material->textures[FBXMaterial::DiffuseTexture] != nullptr )
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		}

		if ( mesh->m_material->textures[FBXMaterial::NormalTexture] != nullptr )
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::NormalTexture]->handle);
		}

		if ( mesh->m_material->textures[FBXMaterial::SpecularTexture] != nullptr )
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::SpecularTexture]->handle);
		}

		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	
}

void FBXObject::CreateOpenGLBuffers()
{
	// create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		// storage for the opengl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // normal
		glEnableVertexAttribArray(2); // texture

		if (m_animated)
		{
			glEnableVertexAttribArray(3); // weights
			glEnableVertexAttribArray(4); // indices
		}

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,	sizeof(FBXVertex), ((char*)0) + FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

		if (m_animated)
		{
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,	sizeof(FBXVertex), ((char*)0) + FBXVertex::WeightsOffset);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::IndicesOffset);
		}

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh->m_userData = glData;
	}
}

void FBXObject::CleanupOpenGLBuffers() 
{
	// clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) 
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}