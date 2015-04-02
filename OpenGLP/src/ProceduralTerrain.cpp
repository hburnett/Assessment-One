#include <ProceduralTerrain.h>
#include <stb_image.h>
#include <random>

Perlin::Perlin(unsigned int dimensions, unsigned int program, float amplitude, float persistence)
{
	m_dimensions = dimensions;
	m_programID = program;
	m_amplitude = amplitude;
	m_persistence = persistence;
	unsigned int octaves = 6;

	srand(time(NULL));

	GenerateGrid();

	m_water = LoadTexture("resources/textures/procedural/water.jpg");
	m_sand = LoadTexture("resources/textures/procedural/sand.jpg");
	m_grass = LoadTexture("resources/textures/procedural/grass.jpg");
	m_snow = LoadTexture("resources/textures/procedural/snow.jpg");	


	float scale = (1.0f / m_dimensions) * 3;

	GeneratePerlinNoise(octaves, scale, m_amplitude, m_persistence);

	
	

	
}

void Perlin::GenerateGrid() 
{
	unsigned int rows = m_dimensions;
	unsigned int cols = m_dimensions;


	ProceduralVertex* aoVertices = new ProceduralVertex[ rows * cols ];
	
	for ( unsigned int r = 0 ; r < rows ; ++r ) 
	{
		for ( unsigned int c = 0 ; c < cols; ++c ) 
		{
			aoVertices[ r * cols + c ].position = glm::vec4((float)c, 0, (float)r, 1);
			// x % 2 = odd

			// colour
			aoVertices[ r * cols + c ].texCoords = glm::vec2((float)c / cols, (float)r / rows);
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[ (rows - 1) * (cols - 1) * 6 ];
	unsigned int index = 0;
	for ( unsigned int r = 0 ; r < (rows - 1) ; ++r ) 
	{
		for ( unsigned int c = 0 ; c < (cols - 1); ++c ) 
		{
			// triangle 1
			auiIndices[ index++ ] = r * cols + c;
			auiIndices[ index++ ] = (r + 1) * cols + c;
			auiIndices[ index++ ] = (r + 1) * cols + (c + 1);
			// triangle 2
			auiIndices[ index++ ] = r * cols + c;
			auiIndices[ index++ ] = (r + 1) * cols + (c + 1);
			auiIndices[ index++ ] = r * cols + (c + 1);
		}
	}

	m_indexCount = index;
	
	// Create VAO, VBO and IBO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	// Bind buffers
	glBindVertexArray( m_VAO );
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	// send data to the buffers	
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(ProceduralVertex), aoVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
	
	// describe how the vertices are setup so they can be sent to the shader
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ProceduralVertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ProceduralVertex), (void*)(sizeof(glm::vec4)));

	// m_VAO hold all our ARRAY_BUFFER and ARRAY_ELEMENT_BUFFER settings
	// so just rebind it later before using glDrawElements
	glBindVertexArray(0);
		
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	delete[] aoVertices;
	delete[] auiIndices;
}

void Perlin::GeneratePerlinNoise(int octaves, float scale, float ampl, float pers)
{	
	float HI = 4294967295;
	float LO = 0;	
	unsigned int seed = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));

	float *perlin_data = new float[m_dimensions * m_dimensions];

	for (unsigned int x = 0; x < m_dimensions; ++x)
	{
		for (unsigned int y = 0; y < m_dimensions; ++y)
		{
			float amplitude = ampl;
			float persistence = pers;
			perlin_data[y * m_dimensions + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{

				float freq = powf(2, (float)o);
				float perlin_sample = glm::perlin(glm::vec3((float)x, (float)y, seed) * scale * freq) * 0.5f + 0.5f;
				
				perlin_data[y * m_dimensions + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}

	glGenTextures(1, &m_perlinTexture);
	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_dimensions, m_dimensions, 0, GL_RED, GL_FLOAT, perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


GLuint Perlin::LoadTexture(char * filename)
{
	GLuint tex = 0;
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &tex); 
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);		

	return tex;
}

void Perlin::Draw(FlyCamera *camera)
{
	glUseProgram(m_programID);
	
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);
	
	glActiveTexture(GL_TEXTURE1); 
	glBindTexture(GL_TEXTURE_2D, m_water);
	
	glActiveTexture(GL_TEXTURE2); 
	glBindTexture(GL_TEXTURE_2D, m_sand);
	
	glActiveTexture(GL_TEXTURE3); 
	glBindTexture(GL_TEXTURE_2D, m_grass);
	
	glActiveTexture(GL_TEXTURE4); 
	glBindTexture(GL_TEXTURE_2D, m_snow);

	unsigned int location = glGetUniformLocation(m_programID, "perlin_texture");
	glUniform1i(location, 0);

	location = glGetUniformLocation(m_programID, "waterTexture");
	glUniform1i(location, 1);
	
	location = glGetUniformLocation(m_programID, "sandTexture");
	glUniform1i(location, 2);
	
	location = glGetUniformLocation(m_programID, "grassTexture");
	glUniform1i(location, 3);
	
	location = glGetUniformLocation(m_programID, "snowTexture");
	glUniform1i(location, 4);

	location = glGetUniformLocation(m_programID,"ProjectionView");
	glUniformMatrix4fv(location, 1, false,	glm::value_ptr(camera->GetProjectionView()));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

