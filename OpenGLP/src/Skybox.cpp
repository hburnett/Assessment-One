#include <Skybox.h>
#include <stb_image.h>


unsigned int Skybox::LoadShader(unsigned int type, const char* path) 
{
	FILE* file = fopen(path, "rb");

	if (file == nullptr)
		return 0;

	// read the shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = new char[length + 1];

	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);

	delete[] source;

	return shader;
}

void Skybox::LoadTexture(GLenum orientation, const char* filename)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glTexImage2D(orientation, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);	
}

Skybox::Skybox(std::string folderDir)
{	
	glGenTextures(1, &m_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

	std::string texFileName = folderDir + "right.jpg";
	LoadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, texFileName.c_str());
		
	texFileName = folderDir + "left.jpg";
	LoadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, texFileName.c_str());
	
	texFileName = folderDir + "top.jpg";
	LoadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, texFileName.c_str());
	
	texFileName = folderDir + "bottom.jpg";
	LoadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, texFileName.c_str());
	
	texFileName = folderDir + "back.jpg";
	LoadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, texFileName.c_str());
	
	texFileName = folderDir + "front.jpg";
	LoadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, texFileName.c_str());

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	unsigned int vsSky = LoadShader(GL_VERTEX_SHADER, "resources/shaders/skybox.vert");
	unsigned int fsSky = LoadShader(GL_FRAGMENT_SHADER, "resources/shaders/skybox.frag");

	int successSky = GL_FALSE;
	
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vsSky);
	glAttachShader(m_programID, fsSky);
	glLinkProgram(m_programID);
	glGetProgramiv(m_programID, GL_LINK_STATUS, &successSky);

	if (successSky == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader CPU particle program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fsSky);
	glDeleteShader(vsSky);	

	//// Create VAO, VBO and IBO
	//glGenVertexArrays(1, &m_vao);
	//// Bind buffers
	//glBindVertexArray( m_vao );
	//
	//// send data to the buffers	
	//
	//// describe how the vertices are setup so they can be sent to the shader
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
	//
	//// m_VAO hold all our ARRAY_BUFFER and ARRAY_ELEMENT_BUFFER settings
	//// so just rebind it later before using glDrawElements
	//glBindVertexArray(0);

	
	GLfloat skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

}


void Skybox::Draw(FlyCamera *m_camera)
{
	glDepthMask(GL_FALSE);
	glUseProgram(m_programID);

	int loc = glGetUniformLocation(m_programID, "projView");
	glm::mat4 view = glm::mat4(glm::mat3(m_camera->GetView()));
	glm::mat4 projection = m_camera->GetProjection();
	glm::mat4 projectionView = projection * view;

	glUniformMatrix4fv(loc, 1, false, &projectionView[0][0]);
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(m_programID, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}







