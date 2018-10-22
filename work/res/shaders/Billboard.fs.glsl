#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

uniform sampler2D myTextureSampler;

uniform float LifeLevel;

void main(){
	// Output color = color of the texture at the specified UV
	color = texture( myTextureSampler, UV );
	 if(color.a < 0.1)
        discard;
	// Hardcoded life level, should be in a separate texture.
}