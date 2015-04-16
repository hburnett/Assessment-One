#version 410		
							
in vec2 vTexCoord;			
in vec4 vNormal;			
in vec4 vPosition;	
							
out vec4 FragColor;		
							
uniform vec3 LightPos;		
uniform vec3 LightColour;	
uniform vec3 CameraPos;		
uniform float SpecPow;		
uniform sampler2D diffuse;	
						
void main() 
{  
	vec3 LightDir = normalize(LightPos.xyz - vPosition.xyz);
	float d = max(0, dot( normalize(vNormal.xyz), LightDir));
	vec3 E = normalize( CameraPos - vPosition.xyz );
	vec3 R = reflect( -LightDir, vNormal.xyz );	
	float s = max( 0, dot( E, R ) );	
	s = pow( s, SpecPow );	
	FragColor = texture(diffuse,vTexCoord) * (vec4((LightColour * d) + (LightColour * s), 1));
};
