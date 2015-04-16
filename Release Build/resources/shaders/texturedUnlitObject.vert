#version 410						
layout(location=0) in vec4 Position;								
layout(location=1) in vec2 TexCoord;			
layout(location=2) in vec4 Normal;		
			
out vec4 vPosition;	
out vec2 vTexCoord;					
out float vHeight;					
out vec4 vNormal;		
			
uniform mat4 ProjectionView;	
uniform sampler2D perlin_texture;
uniform float dimensions;

void main() 
{ 	
	vec4 pos = Position;			
	vNormal = Normal;		
	vHeight = 40;
		
	pos.y += texture(perlin_texture, TexCoord).r * 5;
	
	if (pos.y < 0.7f * vHeight)
		pos.y = 0.698f * vHeight;
	
	pos.x -= dimensions / 2;
	pos.z -= dimensions / 2;
	vPosition = pos;				
	vTexCoord = TexCoord;							
	gl_Position = ProjectionView * pos; 
};