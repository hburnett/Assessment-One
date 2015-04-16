#version 410

layout(location=0) in vec4 Position; 
layout(location=1) in vec4 Colour; 

out vec4 vNormal; 
out vec4 vPosition; 
out vec4 vColour;
uniform mat4 ProjectionView; 

void main() 
{ 
	vec4 pos = Position;
	pos.y += 100;
	vPosition = pos; 
	vColour = Colour;  
	gl_Position = ProjectionView * pos;	
};