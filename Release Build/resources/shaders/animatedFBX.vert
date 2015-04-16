#version 410															
layout(location=0) in vec4 Position;										
layout(location=1) in vec4 Normal;										
layout(location=2) in vec2 TexCoord;										
layout(location=3) in vec4 Weights;										
layout(location=4) in vec4 Indices;	

out vec4 vPosition;														
out vec4 vNormal;														
out vec2 vTexCoord;					

uniform mat4 ProjectionView;												
uniform mat4 global;													
const int MAX_BONES = 92;												
uniform mat4 bones[MAX_BONES];	
										
void main() 
{	
	vPosition = Position;									
	vNormal = Normal;										
	vTexCoord = TexCoord;									
	ivec4 index = ivec4(Indices);							
	vec4 P = bones[ index.x ] * Position * Weights.x;		
	P += bones[ index.y ] * Position * Weights.y;			
	P += bones[ index.z ] * Position * Weights.z;			
	P += bones[ index.w ] * Position * Weights.w;			
	P.w = 1;												
	gl_Position = ProjectionView * global * P;
}								