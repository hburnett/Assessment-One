#version 410						
layout(location=0) in vec4 Position;								
layout(location=1) in vec2 TexCoord;	
			
out vec4 vPosition;	
out vec2 vTexCoord;					
			
uniform mat4 ProjectionView;	
uniform sampler2D perlin_texture;

void main() 
{ 	
	vec4 pos = Position;
	pos.y += texture(perlin_texture, TexCoord).r * 5;
	vPosition = pos;				
	vTexCoord = TexCoord;							
	gl_Position = ProjectionView * pos; 
};