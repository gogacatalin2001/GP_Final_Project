#version 410 core

// set the pointers to the vetrex attribute locations
layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

// outputs to fragment shader
out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;

// uniforms
// Camera transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	// apply the transformations to the vertex position
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	// send the vertex attributes to the fragment shader
	fPosition = vPosition;
	fNormal = vNormal;
	fTexCoords = vTexCoords;
}
