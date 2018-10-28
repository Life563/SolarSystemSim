#pragma once
#include <random>
#include <chrono>


#include "cgra/mesh.hpp"
#include "cgra/shader.hpp"

#include "cgra/matrix.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"

#include <map>

using namespace std;

struct PlanetInfo {
	glm::vec3 location;
	std::vector<glm::vec3> colorSet1;
	float rotationSpeed;
};

class Planet {
public:

	// Methods
	Planet();
	Planet(PlanetInfo pi, int id, int octs, float freq, float amp, int sub );
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
	// Ones Used after the height map
	std::vector<glm::vec3> modifiedVerticies;

	// Color Pallets
	std::vector<glm::vec3> cs1;

	// Sites used for voronoi
	std::vector<glm::vec3> sites;
	std::vector<int> water; // 0 means no water, 1 means water

	std::map<int, int> midPoints;
	std::vector<glm::vec3> vertColours; // Current colors for each vertex

	std::string name;
	int planetId;
	bool selected;
	int radius;
	int subdivisions = 1;
	std::vector<int> biomeMap;
	bool perlin = true, simplex = false;
	bool hasMoon = false;
	bool hasRing = false;

	int octaves = 4;
	float frequency = 3;
	float amplitude = 2;

	float waterDepth = 1.0f;
	float ringAngle = 1.0f;

	double timeTaken;

	//TREE STUFF
	int amtTrees;
	default_random_engine randGen = default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
	std::vector<int> treeVerts;

	// Methods
	void generatePlanet();
	void generateIcosahedron();
	void subdivideIcosahedron();
	int getMidPoint(int a, int b);
	void generateTerrain();
	void generateMoon();
	void generateRings();
	void populatePlanet();
	void voronoiCells(bool genNewSites);

	// Noise
	float generateNoise(int i);


};
