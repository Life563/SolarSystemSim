#pragma once

#include "opengl.hpp"
#include <vector>

#include "cgra/shader.hpp"
#include "glm/glm.hpp"

// The Basic light struct. All types of lights are derived from this one.
struct BaseLight {
	glm::vec3 color;
	float intensity;
};

// Point light struct derived from BaseLight struct.
struct PointLight : public BaseLight {
	glm::vec3 position;
};

// Struct representing directional light
struct DirectionalLight : public BaseLight {
	glm::vec3 direction;
};

// A Class that represents the lighting in the scene
class LightScene {
	cgra::Program m_program;

	static const int NUM_POINT_LIGHTS = 5;
	// An array of point light locations and their member variables locations.
	// Used to easily reference uniform variables in the shader. 
	struct {
		GLuint color;
		GLuint intensity;
		GLuint position;
	} m_pointLightsLocation[NUM_POINT_LIGHTS];

	// Struct used to represent the GL locations of directional light attributes.
	struct {
		GLuint color;
		GLuint intensity;
		GLuint direction;
	} m_dirLightLocation;

	// The single directional light in the scene. Represents the sun.
	DirectionalLight m_directionalLight;

	GLuint m_numPointLightsLocation;


public:
	LightScene();
	// Constructor - Requires shader program being used in scene
	LightScene(cgra::Program shader);

	// Initializes the lightScene with correct GL locations
	void init();

	// Updates point light values in the shader
	void setPointLights(unsigned int numLights, const PointLight * pLights);

	//Updates the directional light value in the shader
	void setDirectionalLight(const DirectionalLight& light);
};