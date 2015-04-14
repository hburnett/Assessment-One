#include <Grid.h>

Grid::Grid(TweakBar *tweaks, unsigned int programID)
{
	m_tweaks = tweaks;
	m_programID = programID;
}

void Grid::GenerateGrid() 
{
	unsigned int rows = 64;
	unsigned int cols = 64;

	Vertex* aoVertices = new Vertex[ rows * cols ];
	
	for ( unsigned int r = 0 ; r < rows ; ++r ) 
	{
		for ( unsigned int c = 0 ; c < cols ; ++c ) 
		{
			aoVertices[ r * cols + c ].position = glm::vec4((float)c, 0, (float)r, 1);
			// x % 2 = odd

			// colour
			aoVertices[ r * cols + c ].colour = glm::vec4(1);
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[ (rows - 1) * (cols - 1) * 6 ];
	unsigned int index = 0;
	for ( unsigned int r = 0 ; r < (rows - 1) ; ++r ) 
	{
		for ( unsigned int c = 0 ; c < (cols - 1) ; ++c ) 
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
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
	
	// describe how the vertices are setup so they can be sent to the shader
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	// m_VAO hold all our ARRAY_BUFFER and ARRAY_ELEMENT_BUFFER settings
	// so just rebind it later before using glDrawElements
	glBindVertexArray(0);
		
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	delete[] aoVertices;
	delete[] auiIndices;
}

void Grid::Draw(FlyCamera *camera)
{
	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID,"ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false,	glm::value_ptr(camera->GetProjectionView()));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

