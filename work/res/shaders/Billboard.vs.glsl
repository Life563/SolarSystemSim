#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 2) in vec3 squareVertices;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 modelMat; 
uniform mat4 viewMat; 
uniform mat4 projectionMat; 
uniform vec2 BillboardSize; // Size of the billboard, in world units (probably meters)

void main()
{
	
		vec3 vertexPosition_worldspace = 
		CameraRight_worldspace * squareVertices.x * BillboardSize.x 
		+ CameraUp_worldspace * squareVertices.y * BillboardSize.y;



	// Output position of the vertex
	gl_Position = projectionMat * viewMat * modelMat * vec4(vertexPosition_worldspace, 1.0f);

	UV = squareVertices.xy + vec2(0.5, 0.5);
}
