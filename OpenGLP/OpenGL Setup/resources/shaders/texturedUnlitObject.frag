#version 410		

in vec2 vTexCoord;																						
in vec4 vPosition;	
in float vHeight;

out vec4 FragColor;		

uniform sampler2D perlin_texture;
uniform sampler2D waterTexture;
uniform sampler2D sandTexture;
uniform sampler2D grassTexture;
uniform sampler2D snowTexture;

void main() 
{
	vec4 col = vec4(1,1,1,1);
	
	vec4 water, sand, grass, snow;
	
	float minWater = 0.7f;
	float minSand = 0.71f;
	float maxSand = 0.74f;
	float minGrass = 0.76f;
	float maxGrass = 0.86f;
	float minSnow = 0.89f;
	
	minWater  *= vHeight;
	minSand   *= vHeight;
	maxSand   *= vHeight;
	minGrass  *= vHeight;
	maxGrass  *= vHeight;
	minSnow   *= vHeight;
	
	
 	water = texture2D(waterTexture, vec2(vTexCoord.x * 15, vTexCoord.y * 15));
 	sand  = texture2D(sandTexture,  vec2(vTexCoord.x * 20, vTexCoord.y * 20));
 	grass = texture2D(grassTexture, vec2(vTexCoord.x * 10, vTexCoord.y * 10));
	snow  = texture2D(snowTexture,  vec2(vTexCoord.x * 4, vTexCoord.y * 4));
	
	if ( vPosition.y <= minWater)
		col = water;
		
	if ( vPosition.y < minSand && vPosition.y > minWater)		
	{
		float dif = ((minSand - minWater) / vHeight) * 100;
		col = mix(water, sand, ((vPosition.y) - minWater) * (2.5f / dif));
	}
		
	if ( vPosition.y < maxSand * vHeight  && vPosition.y > minSand)
		col = sand;
	
	if ( vPosition.y < minGrass && vPosition.y > maxSand)		
	{
		float dif = ((minGrass - maxSand) / vHeight) * 100;
		col = mix(sand, grass, ((vPosition.y) - maxSand) * (2.5f / dif));	
	}
	
	if ( vPosition.y < maxGrass && vPosition.y > minGrass)
		col = grass;
		
	if ( vPosition.y < minSnow && vPosition.y > maxGrass)
	{
		float dif = ((minSnow - maxGrass) / vHeight) * 100;
		col = mix(grass, snow, ((vPosition.y) - maxGrass) * (2.5f / dif));
	}
		
	if ( vPosition.y > minSnow )
		col = snow;
	
	//vec4 fC = vec4(1,1,1,1);//texture(perlin_texture,vTexCoord).rrrr;

	col.a = 1;
	
	FragColor = col;
};

