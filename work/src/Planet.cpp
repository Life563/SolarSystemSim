#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <random>

#include "opengl.hpp"
#include "imgui.h"

#include "cgra/matrix.hpp"
#include "cgra/wavefront.hpp"

#include "Planet.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

//  Windows
#ifdef _WIN32

#include <intrin.h>
uint64_t rdtsc() {
	return __rdtsc();
}

//  Linux/GCC
#else

uint64_t rdtsc() {
	unsigned int lo, hi;
	__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32) | lo;
}

#endif

/**
 * Constructor class. This is treated as the sun
 **/
Planet::Planet() {
	// Generate the Planet
	generateIcosahedron();
	subdivideIcosahedron();
	// Set mesh
	vertColours.clear();
	std::random_device rd;
	std::mt19937 gen(rd());
	cgra::Matrix<double> vertices(originalVerticies.size(), 3);
	for (unsigned int i = 0; i < originalVerticies.size(); i++) {
		vertices.setRow(i, { originalVerticies.at(i)[0], originalVerticies.at(i)[1], originalVerticies.at(i)[2] });
		std::uniform_int_distribution<> dis(150, 255);
		float g = dis(gen);
		vertColours.push_back({ 255.0f/255.0f, g /255.0f, 0.0f /255.0f }); // Shift the green value for some variation
	}
	// Setup Triangles
	cgra::Matrix<unsigned int> triangles(originalTriangles.size(), 3);
	for (unsigned int i = 0; i < originalTriangles.size(); i++) {
		triangles.setRow(i, { originalTriangles.at(i)[0], originalTriangles.at(i)[1], originalTriangles.at(i)[2] });
	}
	// Set Mesh
	this->mesh.setData(vertices, triangles, this->vertColours);
}

/*
* Main constructor for planets
*/
Planet::Planet(PlanetInfo pi, int id, int octs, float freq, float amps, int sub){
	// Load info from planet info class
	this->location = pi.location;
	this->rotationSpeed = pi.rotationSpeed;
	this->cs1 = pi.colorSet1;
	// Extras
	this->planetId = id;
	this->subdivisions = sub;
	// Noise Values
	this->octaves = octs;
	this->frequency = freq;
	this->amplitude = amps;
	// Generate the Planet
	generatePlanet();
}

/*
* Creates the base icosahedron
*/
void Planet::generateIcosahedron() {
	// Setup Verticies
	float t = (1.0f + glm::sqrt(5.0f)) / 2.0f;
	originalVerticies = { glm::normalize(glm::vec3(-1.0f, t, 0.0f)), glm::normalize(glm::vec3(1.0f, t, 0.0f)), glm::normalize(glm::vec3(-1.0f, -t, 0.0f)), glm::normalize(glm::vec3(1.0f, -t, 0.0f)), glm::normalize(glm::vec3(0.0f, -1.0f, t)), glm::normalize(glm::vec3(0.0f, 1.0f, t)), glm::normalize(glm::vec3(0.0f, -1.0f, -t)), glm::normalize(glm::vec3(0.0f, 1.0f, -t)), glm::normalize(glm::vec3(t, 0.0f, -1.0f)), glm::normalize(glm::vec3(t, 0.0f, 1.0f)), glm::normalize(glm::vec3(-t, 0.0f, -1.0f)), glm::normalize(glm::vec3(-t, 0.0f, 1.0f)) };
	originalTriangles = { { 0, 11, 5 }, { 0, 5, 1 }, {0, 1, 7}, {0, 7, 10}, {0, 10, 11}, {1,5,9},{5, 11,4}, {11, 10,2}, {10, 7,6}, {7, 1,8}, {3, 9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9}, {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1} };
}

/*
* Method to generate the planets
*/
void Planet::generatePlanet() {
	this->hasMoon = false;
	this->hasRing = false;
	srand((unsigned int)rdtsc()); // Makes sure we have a random seed
	double startTime = glfwGetTime();
	generateIcosahedron();
	subdivideIcosahedron();
	generateTerrain();

	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_real_distribution<double> distribution(0.0, 1.0);
	// Do we want to generate other planet features?
	float point = distribution(randGen);
	if (point < 0.5f) { // Generate a moon
		this->generateMoon();
	}
	point = distribution(randGen);
	if (point < 0.5f) { // Generate a ring around planet
		this->generateRings();
	}
	//TREES
	std::uniform_int_distribution<> dis(0, 4);
	amtTrees = dis(gen);
	for (int i = 0; i < amtTrees;i++) {
		int seed = std::chrono::system_clock::now().time_since_epoch().count();
		randGen = default_random_engine(seed);
		uniform_real_distribution<double> dis(0.0, modifiedVerticies.size()-1);
		int tv = dis(randGen);
		treeVerts.push_back(tv);
	}
	std::cout << this->originalVerticies.size() << std::endl;
	this->timeTaken = glfwGetTime() - startTime;
}

/*
* Every triangle when subdivided will turn into 4 triangles
*/
void Planet::subdivideIcosahedron() {
	midPoints = std::map<int, int>();
	for (int i = 0; i < this->subdivisions; i++) {
		std::vector<std::vector<unsigned int>> newTris;
		for (std::vector<unsigned int> tri : this->originalTriangles) { // Cycle through each polygon
			// Create a new vertex or find one that was previously made
			unsigned int ab = getMidPoint(tri[0], tri[1]);
			unsigned int bc = getMidPoint(tri[1], tri[2]);
			unsigned int ca = getMidPoint(tri[2], tri[0]);
			// Create Polygons
			newTris.push_back({ tri[0], ab, ca});
			newTris.push_back({ tri[1], bc, ab});
			newTris.push_back({ tri[2], ca, bc});
			newTris.push_back({ab, bc, ca});
		}
		// Update List
		this->originalTriangles = newTris;
	}
}

/*
* Method to get the mid point between 2 vertices. Used when subdividing the icosahedron
*/
int Planet::getMidPoint(int a, int b) {
	int smallerIndex = glm::min(a, b);
	int greaterIndex = glm::max(a, b);
	int key = (smallerIndex << 16) + greaterIndex;
	// Is a midpoint already avaliable to use? The return that
	std::map<int, int>::iterator i = midPoints.find(key);
	if (i != midPoints.end()) {
		return midPoints.at(key);
	}
	// Need to create a new midpoint
	glm::vec3 p1 = originalVerticies.at(smallerIndex);
	glm::vec3 p2 = originalVerticies.at(greaterIndex);
	glm::vec3 mid = glm::normalize(glm::lerp(p1, p2, 0.5f));

	int loc = originalVerticies.size();
	originalVerticies.push_back({mid.x, mid.y, mid.z});

	midPoints.insert(std::pair<int, int>(key, loc));
	return loc;
}

/*
*  Returns a floating point between -1.0f & 1.0f which has been generated through running a vertex point through a noise algorithm
*/
float Planet::generateNoise(int i) {
	std::random_device rd;
	std::mt19937 gen(rd());
	uniform_real_distribution<double> distribution(0, 1.0);
	// 1.0f / (glm::sqrt(originalVerticies.size()) - 1);
	float xFactor = distribution(gen);
	float yFactor = distribution(gen);
	float zFactor = distribution(gen);

	float x = xFactor * originalVerticies.at(i)[0];
	float y = yFactor * originalVerticies.at(i)[1];
	float z = zFactor * originalVerticies.at(i)[2];

	float sum = 0.0f;
	// Tuneable
	float freq = this->frequency;
	float amp = this->amplitude;
	for (int octs = 0; octs < this->octaves; octs++) {
		glm::vec3 p = glm::vec3(x * freq, y*freq, z * freq);
		float value;
		if (this->perlin) {
			value = glm::perlin(p, glm::vec3(freq)) / amp;
		} else if (this->simplex) {
			value = glm::simplex(p) / amp;
		} else { // Random
			uniform_real_distribution<double> rDistribution(-1.0, 1.0);
			value = rDistribution(gen);
		}
		sum += value;
		freq *= 2.0f;
		amp *= 2;
	}
	return sum;
}

/*
* Biome Keys:
* 0 = FlatLand
* 1 = Desert
* 2 = Snow
* 3 = Jungle
* 4 = Urban
*/
void Planet::generateTerrain() {
	// Generate and apply noise to change terrain
	modifiedVerticies.clear();
	for (unsigned int i = 0; i < originalVerticies.size(); i++) {
		float point = this->generateNoise(i);
		if (point < 0.0f) {
			point = 0.0f;
		}
		if (point > 0.3f) {
			point = 0.3f;
		}
		modifiedVerticies.push_back(originalVerticies.at(i) * (glm::length(originalVerticies.at(i)) + point));
	}
	// Use Vor Cells to generate biomes
	voronoiCells(true);
	// Decide on water
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> waterDistribution(0, originalVerticies.size() - 1);
	std::uniform_int_distribution<> seaDistribution(-15, 15);
	for (int i = 0; i < originalVerticies.size(); i++) {
		// Generate a point
		float p = waterDistribution(gen);
		float point = glm::perlin((originalVerticies.at(p)));
		if (point < this->waterDepth) {
			float oc = seaDistribution(gen);
			vertColours.at(i) = glm::vec3((this->cs1.at(0).x + oc) / 255, (this->cs1.at(0).y + oc) / 255, (this->cs1.at(0).z + oc) / 255); // 'Sea' color
		}
	}
	// Set mesh
	cgra::Matrix<double> vertices(modifiedVerticies.size(), 3);
	for (unsigned int i = 0; i < modifiedVerticies.size(); i++) {
		vertices.setRow(i, { modifiedVerticies.at(i)[0], modifiedVerticies.at(i)[1], modifiedVerticies.at(i)[2] });
	}
	// Setup Triangles
	cgra::Matrix<unsigned int> triangles(originalTriangles.size(), 3);
	for (unsigned int i = 0; i < originalTriangles.size(); i++) {
		triangles.setRow(i, { originalTriangles.at(i)[0], originalTriangles.at(i)[1], originalTriangles.at(i)[2] });
	}
	// Set Mesh
	this->mesh.setData(vertices, triangles, this->vertColours);
}

/*
* An algorithm to determin which points are part of which biome. A site is a starter point for a biome
*/
void Planet::voronoiCells(bool genNewSites) {
	std::random_device rd;
	std::mt19937 gen(rd());
	if (genNewSites) {
		// Determin which points are going to become main sites
		std::uniform_int_distribution<> dis(0, this->modifiedVerticies.size()-1);
		int numberOfSites = 5;
		for (int i = 0; i < numberOfSites; i++) {
			// Decide on a random point
			int vertToSite = dis(gen);
			this->sites.push_back(this->modifiedVerticies.at(vertToSite));
		}
	}
	vertColours.clear();
	// Now Decided on the biome of each vertex, as well as the color
	std::uniform_int_distribution<> distribution(-15, 15);
	for (unsigned int i = 0; i < this->modifiedVerticies.size(); i++) {
		float shortestDistance = FLT_MAX;
		int biomeNum;
		// Check list of sites
		for (unsigned int j = 0; j < this->sites.size(); j++) {
			float dis = glm::abs(glm::distance(modifiedVerticies.at(i), this->sites.at(j)));
			if (dis == shortestDistance) {
				// Randomly decide on the winner, this helps with the borders between sites
				std::uniform_real_distribution<double> siteDist(0.0, 1.0);
				double dp = siteDist(gen);
				if (dp < 0.5f) {
					shortestDistance = dis;
					biomeNum = j;
				}
			}else if (dis <= shortestDistance) {
				shortestDistance = dis;
				biomeNum = j;
			}
		}
		// Shift the colors slightly for some variance
		float offsetColor = distribution(gen);
		// Get the color of the decided biome
		if (biomeNum == 0) { // Flatlands
			vertColours.push_back({ (this->cs1.at(1).x + offsetColor) / 255, (this->cs1.at(1).y + offsetColor) / 255, (this->cs1.at(1).z + offsetColor) / 255 });
		}
		else if (biomeNum == 1) { // Dessert
			vertColours.push_back({ (this->cs1.at(2).x + offsetColor) / 255, (this->cs1.at(2).y + offsetColor) / 255, (this->cs1.at(2).z + offsetColor) / 255 });
		}
		else if (biomeNum == 2) { // Snow
			vertColours.push_back({ (this->cs1.at(3).x + offsetColor) / 255, (this->cs1.at(3).y + offsetColor) / 255, (this->cs1.at(3).z + offsetColor) / 255 });
		}
		else if (biomeNum == 3) { // Jungle
			vertColours.push_back({ (this->cs1.at(4).x + offsetColor) / 255, (this->cs1.at(4).y + offsetColor) / 255, (this->cs1.at(4).z + offsetColor) / 255 });
		}
		else { // Urban
			vertColours.push_back({ (this->cs1.at(5).x + offsetColor) / 255, (this->cs1.at(5).y + offsetColor) / 255, (this->cs1.at(5).z + offsetColor) / 255 });
		}
		biomeMap.push_back(biomeNum);
	}	
}

/*
* Generates a small moon for the planet which uses the base icohedron
*/
void Planet::generateMoon() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-15, 15);
	this->hasMoon = true;
	float t = (1.0f + glm::sqrt(5.0f)) / 2.0f;
	std::vector<glm::vec3> moonVertices = { glm::normalize(glm::vec3(-1.0f, t, 0.0f)), glm::normalize(glm::vec3(1.0f, t, 0.0f)), glm::normalize(glm::vec3(-1.0f, -t, 0.0f)), glm::normalize(glm::vec3(1.0f, -t, 0.0f)), glm::normalize(glm::vec3(0.0f, -1.0f, t)), glm::normalize(glm::vec3(0.0f, 1.0f, t)), glm::normalize(glm::vec3(0.0f, -1.0f, -t)), glm::normalize(glm::vec3(0.0f, 1.0f, -t)), glm::normalize(glm::vec3(t, 0.0f, -1.0f)), glm::normalize(glm::vec3(t, 0.0f, 1.0f)), glm::normalize(glm::vec3(-t, 0.0f, -1.0f)), glm::normalize(glm::vec3(-t, 0.0f, 1.0f)) };
	std::vector<std::vector<unsigned int>> moonTriangles = { { 0, 11, 5 }, { 0, 5, 1 }, {0, 1, 7}, {0, 7, 10}, {0, 10, 11}, {1,5,9},{5, 11,4}, {11, 10,2}, {10, 7,6}, {7, 1,8}, {3, 9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9}, {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1} };
	std::vector<glm::vec3> vC; // Color
	cgra::Matrix<double> mV(moonVertices.size(), 3);
	for (unsigned int i = 0; i < moonVertices.size(); i++) {
		float offset = dis(gen);
		mV.setRow(i, { moonVertices.at(i)[0], moonVertices.at(i)[1], moonVertices.at(i)[2] });
		vC.push_back(glm::vec3( (128 + offset) /255, (128 + offset) /255, (128 + offset) /255));
	}
	// Setup Triangles
	cgra::Matrix<unsigned int> mT(moonTriangles.size(), 3);
	for (unsigned int i = 0; i < moonTriangles.size(); i++) {
		mT.setRow(i, { moonTriangles.at(i)[0], moonTriangles.at(i)[1], moonTriangles.at(i)[2] });
	}
	// Set Mesh
	this->moonMesh.setData(mV, mT, vC);
}

/*
*   Method used to generate a ring for the planet. Uses a slightly modified version of the obj loader from 251
*/
void Planet::generateRings() {
	this->hasRing = true;
	cgra::Wavefront obj;
	// Wrap the loading in a try..catch block
	try {
		obj = cgra::Wavefront::load(CGRA_SRCDIR "/res/Ring.obj");
	} catch (std::exception e) {
		std::cerr << "Couldn't load file: '" << e.what() << "'" << std::endl;
	}
	// The mesh data
	 // Replace these with appropriate values
	unsigned int numVertices = obj.m_positions.size();
	unsigned int numTriangles = obj.m_faces.size();

	cgra::Matrix<double> vertices(numVertices, 3);
	cgra::Matrix<unsigned int> triangles(numTriangles, 3);
	std::vector<glm::vec3> vc;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(-15, 15);

	for (size_t i = 0; i < obj.m_positions.size(); i++) {
		// Add each position to the vertices matrix
		vertices.setRow(i, { obj.m_positions[i][0],  obj.m_positions[i][1],  obj.m_positions[i][2] });
		float oc = dist(gen);
		vc.push_back(glm::vec3((150 + oc) / 255, (75+oc) / 255, (15 + oc) / 255));
	}

	for (unsigned int i = 0; i < obj.m_faces.size(); i++) {
		// Add each triangle's indices to the triangles matrix
		// Remember that Wavefront files use indices that start at 1
		std::vector<cgra::Wavefront::Vertex> theVertices = obj.m_faces[i].m_vertices;
		triangles.setRow(i, { theVertices[0].m_p - 1, theVertices[1].m_p - 1, theVertices[2].m_p - 1 });
	}
	std::uniform_real_distribution<double> ra(-2.0, 2.0);
	ringAngle = ra(gen);
	ringMesh.setData(vertices, triangles, vc);
}




void Planet::draw() {
	// Draw the planet mesh first
	this->mesh.draw();
	// Draw the items on the planet afterwards
	populatePlanet();
}

//========================================================//
//================= Alans Methods ========================//
//========================================================//

/*
 * Method used to populate the planet. Alans Main method
 */
void Planet::populatePlanet() {


}
