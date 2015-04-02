#version 410						
layout(location=0) in vec4 Position;								
layout(location=1) in vec2 TexCoord;	
			
out vec4 vPosition;	
out vec2 vTexCoord;					
out float vHeight;
			
uniform mat4 ProjectionView;	
uniform sampler2D perlin_texture;

void main() 
{ 	
	vec4 pos = Position;
	vHeight = 40;
		
	pos.y += texture(perlin_texture, TexCoord).r * 5;
	
	if (pos.y < 0.7f * vHeight)
		pos.y = 0.698f * vHeight;
	
	vPosition = pos;				
	vTexCoord = TexCoord;							
	gl_Position = ProjectionView * pos; 
};