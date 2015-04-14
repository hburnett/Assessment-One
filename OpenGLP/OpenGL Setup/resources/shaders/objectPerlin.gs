#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec2 vUV[];

out vec3 gPosition;
out vec3 gNormal;
out vec2 gUV;

uniform mat4 ProjectionView;
uniform mat4 Transform;

uniform bool HasPerlin;
uniform sampler2D perlinTexture;

void main()
{
	//Get the new normals. :I

	vec3 position =  gl_in[0].gl_Position.xyz;
	vec3 position1 = gl_in[1].gl_Position.xyz;
	vec3 position2 = gl_in[2].gl_Position.xyz;

	gNormal = cross(normalize(position1 - position), normalize(position2 - position));

	gUV = vUV[0];
	gPosition = vPosition[0];
	gl_Position = ProjectionView * gl_in[0].gl_Position;
	EmitVertex();

	gUV = vUV[1];
	gPosition = vPosition[1];
	gl_Position = ProjectionView * gl_in[1].gl_Position;
	EmitVertex();

	gUV = vUV[2];
	gPosition = vPosition[2];
	gl_Position = ProjectionView * gl_in[2].gl_Position;
	EmitVertex();

	//if(HasPerlin)
	//{
	//	float offset = texture(perlinTexture, vUV.xy).r;
	//	vPosition += vNormal * offset;
	//}
}