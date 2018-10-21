#pragma once

#include "cgra/mesh.hpp"
#include "cgra/shader.hpp"

#include "cgra/matrix.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"

#include <map>

extern struct PlanetInfo {
	glm::vec3 location;
	std::vector<std::vector<double>> colorSet1;
	float rotationSpeed;
};

class Planet {
public:
	
	// Methods
	Planet();
	Planet(PlanetInfo pi, int id);
	void draw();

	// Variables
	glm::vec3 location = glm::vec3(0);
	glm::vec3 orbitSpeed;
	glm::vec3 scale = glm::vec3(0.6f);
	float rotationSpeed = 2.0f;

	cgra::Mesh mesh;
	cgra::Mesh moonMesh;
	cgra::Mesh ringMesh;

	// Original
	std::vector<glm::vec3> originalVerticies;
	std::vector<std::vector<unsigned int>> originalTriangles;

	// Sites used for voronoi
	std::vector<glm::vec3> sites;

	std::map<int, int> midPoints;
	std::vector<glm::vec3> vertColours;

	std::string name;
	int colorID; // Used for color picking
	bool selected;
	int radius;
	int subdivisions = 1;
	std::vector<float> heightMap;
	std::vector<float> biomeMap;
	bool random = false, perlin = false, simplex = true;
	bool hasMoon = false;
	bool hasRing = false;

	double timeTaken;

	// Methods
	cgra::Mesh loadObj(const char *filename);
	void generatePlanet();
	void generateIcosahedron();
	void subdivideIcosahedron();
	int getMidPoint(int a, int b);
	void generateTerrain();
	void generateMoon();
	void generateRings();
	void populatePlanet();
	void voronoiCells();

	// Noise Methods
	float generateRandomNoise(int i);
	float generatePerlinNoise(int i);
	float generateSimplexNoise(int i);
	
};
