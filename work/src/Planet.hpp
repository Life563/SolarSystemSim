#pragma once

#include "cgra/mesh.hpp"
#include "cgra/shader.hpp"

#include "glm/glm.hpp"

class Planet {
public:
	// Variables


	// Methods

	Planet();
	void draw();

	// Variables
	glm::vec3 location;
	glm::vec3 orbitSpeed;
	glm::vec3 rotationSpeed;
	glm::vec3 scale;

	cgra::Mesh mesh;
	int radius;
	int subdivisions;

	// Methods
	void generatePlanet();
	void generateTetrahedron();
	void populatePlanet();
	
};
