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
	vec4 pos = Position;
	pos.y += 100;
	vPosition = pos;				
	vNormal = Normal;								
	vTexCoord = TexCoord;				
	gl_Position = ProjectionView * global * pos; 
};
