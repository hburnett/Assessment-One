	#version 410						
layout(location=0) in vec4 Position;				
layout(location=1) in vec4 Normal;				
layout(location=2) in vec2 TexCoord;	
			
out vec4 vPosition;								
out vec4 vNormal;								
out vec2 vTexCoord;					
			
uniform mat4 ProjectionView;						
uniform mat4 global;
								
void main() 
{ 
	Position.y += 100;
	vPosition = Position;				
	vNormal = Normal;								
	vTexCoord = TexCoord;							
	//vec4 pos = vec4(12, 10, 12, 0);
	gl_Position = ProjectionView * global * Position; 
};
