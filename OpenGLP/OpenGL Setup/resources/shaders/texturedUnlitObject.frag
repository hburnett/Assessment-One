#version 410		

in vec2 vTexCoord;																						
in vec4 vPosition;	

out vec4 FragColor;		

uniform sampler2D perlin_texture;

void main() 
{
	vec4 col = vec4(1,1,1,1);
	
	if ( vPosition.y < 5 )
		col = vec4(0,0,1,1);
		
	if ( vPosition.y < 7 && vPosition.y > 5 )
		col = vec4(1,1,0,1);
		
	if ( vPosition.y < 13 && vPosition.y > 7 )
		col = vec4(0,1,0,1);
		
	if ( vPosition.y < 30 && vPosition.y > 22 )
		col = vec4(1,1,1,1);
	
	vec4 fC = vec4(1,1,1,1);//texture(perlin_texture,vTexCoord).rrrr;

	FragColor = fC * col;
};

