#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 2) in vec3 squareVertices;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
uniform mat4 modelMat; 
uniform mat4 viewMat; 
uniform mat4 projectionMat; 
uniform vec3 BillboardPos; // Position of the center of the billboard
uniform vec2 BillboardSize; // Size of the billboard, in world units (probably meters)

void main()
{
	vec3 particleCenter_wordspace = BillboardPos;
	
		vec3 vertexPosition_worldspace = 
		particleCenter_wordspace
		+ CameraRight_worldspace * squareVertices.x * BillboardSize.x 
		+ CameraUp_worldspace * squareVertices.y * BillboardSize.y;



	// Output position of the vertex
	gl_Position = projectionMat * viewMat * modelMat * vec4(vertexPosition_worldspace, 1.0f);

	UV = squareVertices.xy + vec2(0.5, 0.5);
}
