#version 410
	layout(location = 0) in vec3 Position;
	layout(location = 1) in vec3 Velocity;
	layout(location = 2) in float Lifetime;
	layout(location = 3) in float Lifespan;
	
	out vec3 position;
	out vec3 velocity;
	out float lifetime;
	out float lifespan;
	
	uniform float time;
	uniform float deltaTime;
	uniform float lifeMin;
	uniform float lifeMax;
	uniform vec3 emitterPosition;
	
	const float INVERSE_MAX_UINT = 1.0f / 4294967295.0f;
	float rand(uint seed, float range) 
	{
		uint i = (seed ^ 12345391u) * 2654435769u;
		i ^= (i << 6u) ^ (i >> 26u);
		i *= 2654435769u;
		i += (i << 5u) ^ (i >> 12u);
		return float(range * i) * INVERSE_MAX_UINT;
	} 
	
	void main() 
	{
		
		uint seed = uint(time * 1000.0) + uint(gl_VertexID);
		
		velocity = Velocity;
		
		//velocity.x = (pow(1.1f, velocity.x)) - cos(time);
		velocity.y = 0.5f;
		//velocity.z = (pow(1.1f, velocity.z)) + cos(time);
		velocity.x = 0;
		velocity.z = 0;
		
		velocity *= 2;
		
		position = Position + velocity * deltaTime;
		lifetime = Lifetime + deltaTime;
		lifespan = Lifespan;
		
		
		
		// emit a new particle as soon as it dies
		if (lifetime > lifespan) 
		{
			velocity.x = rand(seed++, 2) - 1;
			velocity.y = rand(seed++, 20) - 10;
			velocity.z = rand(seed++, 2) - 1;	
			velocity = normalize(velocity);
			
			float ranAngle = rand(seed++, 6.28f);
			
			position = emitterPosition;	
			position.x += sin(ranAngle) * 20;
			position.z += cos(ranAngle) * 20;
			position.x += rand(seed++, 1) - 0.5f;			
			position.y += rand(seed++, 1) - 0.5f;	
			position.y += 121;	
			position.z += rand(seed++, 1) - 0.5f;	
			
			
			lifetime = 0;
			lifespan = rand(seed++, lifeMax - lifeMin) + lifeMin;
		}
	}