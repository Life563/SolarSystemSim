#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "opengl.hpp"
#include "imgui.h"

#include "cgra/matrix.hpp"
#include "cgra/wavefront.hpp"

#include "Planet.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

/**
 * Class which deals with the planet
 *
 * May create holder classes which contain different types of information about how the planet should be generated, like what colour it would be
 * size etc.
 **/
Planet::Planet() {
	// Generate the Planet
	generateIcosahedron();
	subdivideIcosahedron();
	// Set mesh
	cgra::Matrix<double> vertices(originalVerticies.size(), 3);
	for (int i = 0; i < originalVerticies.size(); i++) {
		vertices.setRow(i, { originalVerticies.at(i)[0], originalVerticies.at(i)[1], originalVerticies.at(i)[2] });
	}
	// Setup Triangles
	cgra::Matrix<unsigned int> triangles(originalTriangles.size(), 3);
	for (int i = 0; i < originalTriangles.size(); i++) {
		triangles.setRow(i, { originalTriangles.at(i)[0], originalTriangles.at(i)[1], originalTriangles.at(i)[2] });
	}
	// Set Mesh
	this->mesh.setData(vertices, triangles);
	std::cout << "Generated Planet" << std::endl;
}


Planet::Planet(PlanetInfo pi, int id){
	this->location = pi.location;
	this->rotationSpeed = pi.rotationSpeed;
	this->colorID = id;
	// Generate the Planet
	generatePlanet();
}


void Planet::generateIcosahedron() {	
	// Setup Verticies
	float t = (1.0f + glm::sqrt(5.0f)) / 2.0f;
	//vs = { { -1.0f, t, 0.0f }, {1.0f, t, 0.0f}, {-1.0f, -t, 0.0f}, {1.0f, -t, 0.0f}, {0.0f, -1.0f, t}, {0.0f, 1.0f, t}, { 0.0f, -1.0f, -t }, { 0.0f, 1.0f, -t }, {t, 0.0f, -1.0f}, {t, 0.0f, 1.0f}, {-t, 0.0f, -1.0f}, {-t, 0.0f, 1.0f} };
	originalVerticies = { glm::normalize(glm::vec3(-1.0f, t, 0.0f)), glm::normalize(glm::vec3(1.0f, t, 0.0f)), glm::normalize(glm::vec3(-1.0f, -t, 0.0f)), glm::normalize(glm::vec3(1.0f, -t, 0.0f)), glm::normalize(glm::vec3(0.0f, -1.0f, t)), glm::normalize(glm::vec3(0.0f, 1.0f, t)), glm::normalize(glm::vec3(0.0f, -1.0f, -t)), glm::normalize(glm::vec3(0.0f, 1.0f, -t)), glm::normalize(glm::vec3(t, 0.0f, -1.0f)), glm::normalize(glm::vec3(t, 0.0f, 1.0f)), glm::normalize(glm::vec3(-t, 0.0f, -1.0f)), glm::normalize(glm::vec3(-t, 0.0f, 1.0f)) };
	//ts = { { 0, 11, 5 }, { 0, 5, 1 }, {0, 1, 7}, {0, 7, 10}, {0, 10, 11}, {1,5,9},{5, 11,4}, {11, 10,2}, {10, 7,6}, {7, 1,8}, {3, 9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9}, {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1} };
	originalTriangles = { { 0, 11, 5 }, { 0, 5, 1 }, {0, 1, 7}, {0, 7, 10}, {0, 10, 11}, {1,5,9},{5, 11,4}, {11, 10,2}, {10, 7,6}, {7, 1,8}, {3, 9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9}, {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1} };
}

/*
* Method
*/
void Planet::generatePlanet() {
	//srand(((float)glfwGetTime() / this->rotationSpeed));
	double startTime = glfwGetTime();
	generateIcosahedron();
	subdivideIcosahedron();
	generateTerrain();
	// Do we want to generate other planet features?
	float point = double(rand()) / (double(RAND_MAX) + 1.0);
	if (point < 0.5f) { // Generate a moon
		this->generateMoon();
	}
	point = double(rand()) / (double(RAND_MAX) + 1.0);
	if (point < 0.5f) { // Generate a ring around planet

	}
	// Set mesh
	cgra::Matrix<double> vertices(originalVerticies.size(), 3);
	for (int i = 0; i < originalVerticies.size(); i++) {
		vertices.setRow(i, { originalVerticies.at(i)[0], originalVerticies.at(i)[1], originalVerticies.at(i)[2] });
	}
	// Setup Triangles
	cgra::Matrix<unsigned int> triangles(originalTriangles.size(), 3);
	for (int i = 0; i < originalTriangles.size(); i++) {
		triangles.setRow(i, { originalTriangles.at(i)[0], originalTriangles.at(i)[1], originalTriangles.at(i)[2] });
	}
	// Set Mesh
	this->mesh.setData(vertices, triangles);
	this->timeTaken = glfwGetTime() - startTime;
	std::cout << "Generated Planet, Time Taken: " << this->timeTaken << " Seconds" << std::endl;
}

/*
* Every triangle when subdivided will turn into 4 triangles
*/
void Planet::subdivideIcosahedron() {
	midPoints = std::map<int, int>();
	for (int i = 0; i < this->subdivisions; i++) {
		std::vector<std::vector<unsigned int>> newTris;
		for each (std::vector<unsigned int> tri in this->originalTriangles) { // Cycle through each polygon	
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

float Planet::generateRandomNoise(int i) {
	return double(rand()) / (double(RAND_MAX) + 1.0);
	//if (point < 0.4f) { // Sea			
	//	// Do nothing
	//	return 0.0f;
	//}else if (point < 0.7) { // Normal
	//	return 0.5f;
	//}else { // Mountain
	//	return 1.0f;
	//}
}

/*
*  Returns a floating point between -1.0f & 1.0f which has been generated through running a vertex point through a Perlin noise algorithm
*/
float Planet::generatePerlinNoise(int i) {
	float scale = rand() % 10 + 1; // Use a scale factor to randomise noise
	float point = glm::perlin(glm::vec3(originalVerticies.at(i)[0]/ scale, originalVerticies.at(i)[1]/ scale, originalVerticies.at(i)[2]/ scale));
	// Update Vertex
	return point;
	//originalVerticies.at(i) = originalVerticies.at(i) * (glm::length(originalVerticies.at(i)) + point/2);
}

/*
* Returns a floating point between -1.0f & 1.0f which has been generated through running a vertex point through a simplex noise algorithm
*/
float Planet::generateSimplexNoise(int i) {
	float scale = rand() % 100 + 1; // Use a scale factor to randomise noise
	float point = glm::simplex(glm::vec3(originalVerticies.at(i)[0] / scale, originalVerticies.at(i)[1] / scale, originalVerticies.at(i)[2] / scale));
	// Update vertex
	return point;
	//originalVerticies.at(i) = originalVerticies.at(i) * (glm::length(originalVerticies.at(i)) + point/2);
}

/*
* Biome Keys:
* 0 = FlatLand (Grass)
* 1 = Desert
* 2 = Snow
* 3 = Jungle
* 4 = Urban

* Terrain Types:
* 0 = Sea
* 1 = Flatlands
* 2 = Hills
* 3 = Mountain
*/
void Planet::generateTerrain() {	
	// Generate and apply noise to change terrain
	for (int i = 0; i < originalVerticies.size(); i++) {
		originalVerticies.at(i) = originalVerticies.at(i) * (glm::length(originalVerticies.at(i)) + generateRandomNoise(i) / 2);
		//originalVerticies.at(i) = originalVerticies.at(i) * (glm::length(originalVerticies.at(i)) + generatePerlinNoise(i) / 2);
		//originalVerticies.at(i) = originalVerticies.at(i) * (glm::length(originalVerticies.at(i)) + generateSimplexNoise(i) / 2);
	}
	// Generate a new set of noise for biome
	for (int i = 0; i < originalVerticies.size(); i++) {
		float point = generateRandomNoise(i);
		//float point = generatePerlinNoise(i);
		//float point = generateSimplexNoise(i);
		//std::cout << point << std::endl;
		if (point > 0.0f && point < 0.5f) { // Grass lands
			biomeMap.push_back(0);
			std::cout << "Grasslands" << std::endl;
		} else if (point < 0.0f && point > -0.5f) { // Desert
			biomeMap.push_back(1);
			std::cout << "Desert" << std::endl;
		} else if (point < -0.5f){  // Snow
			biomeMap.push_back(2);
			std::cout << "Snow" << std::endl;
		}else { // Jungle
			biomeMap.push_back(3); 
			std::cout << "Jungle" << std::endl;
		}
	}
	// Generate Colors based off biome
	for (int i = 0; i < originalVerticies.size(); i++) {
		// Get verticies used for triangle
		glm::vec3 verts = originalVerticies.at(i);
		float r = double(rand()) / (double(RAND_MAX) + 1.0);
		bool land = false, sea = false, mountain = false;
		// Determin Color
		if (r < 0.3f) {
			vertColours.push_back({ 0 / 255, 255 / 255, 0 / 255 });
		}
		else if (r < 0.6f) {
			vertColours.push_back({ 0 / 255, 0 / 255, 255 / 255 });
		}
		else {
			vertColours.push_back({ 255 / 255, 0 / 255, 0 / 255 });
		}
	}
	// Use Vor Cells
	for (int i = 0; i < originalVerticies.size(); i++) {

	}
	// Pass colours to mesh
	this->mesh.setColors(vertColours);
}

/*
* Brute Force Algorithm
* A site is a starter point for a biome
*/
void Planet::voronoiCells() {
	// Determin which points are going to become main sites
	int numberOfSites = rand() % (this->originalVerticies.size()) + 5;
	for (int i = 0; i < numberOfSites; i++) {
		// Decide on a random point
		int vertToSite = rand() % this->originalVerticies.size();
		this->sites.push_back(this->originalVerticies.at(i));
	}
	// Now 
	for (int i = 0; i < this->originalVerticies.size(); i++) {
		float shortestDistance;
		// List of potential canidates
		// If a point is an equal distance between multiple points, randomly decide on what it should be
		
	}
}

void Planet::generateMoon() {
	this->hasMoon = true;
	float t = (1.0f + glm::sqrt(5.0f)) / 2.0f;
	std::vector<glm::vec3> moonVertices = { glm::normalize(glm::vec3(-1.0f, t, 0.0f)), glm::normalize(glm::vec3(1.0f, t, 0.0f)), glm::normalize(glm::vec3(-1.0f, -t, 0.0f)), glm::normalize(glm::vec3(1.0f, -t, 0.0f)), glm::normalize(glm::vec3(0.0f, -1.0f, t)), glm::normalize(glm::vec3(0.0f, 1.0f, t)), glm::normalize(glm::vec3(0.0f, -1.0f, -t)), glm::normalize(glm::vec3(0.0f, 1.0f, -t)), glm::normalize(glm::vec3(t, 0.0f, -1.0f)), glm::normalize(glm::vec3(t, 0.0f, 1.0f)), glm::normalize(glm::vec3(-t, 0.0f, -1.0f)), glm::normalize(glm::vec3(-t, 0.0f, 1.0f)) };
	std::vector<std::vector<unsigned int>> moonTriangles = { { 0, 11, 5 }, { 0, 5, 1 }, {0, 1, 7}, {0, 7, 10}, {0, 10, 11}, {1,5,9},{5, 11,4}, {11, 10,2}, {10, 7,6}, {7, 1,8}, {3, 9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9}, {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1} };

	cgra::Matrix<double> mV(moonVertices.size(), 3);
	for (int i = 0; i < moonVertices.size(); i++) {
		mV.setRow(i, { moonVertices.at(i)[0], moonVertices.at(i)[1], moonVertices.at(i)[2] });
	}
	// Setup Triangles
	cgra::Matrix<unsigned int> mT(moonTriangles.size(), 3);
	for (int i = 0; i < moonTriangles.size(); i++) {
		mT.setRow(i, { moonTriangles.at(i)[0], moonTriangles.at(i)[1], moonTriangles.at(i)[2] });
	}
	// Set Mesh
	this->moonMesh.setData(mV, mT);
}

void Planet::generateRings() {

}


void Planet::draw() {
	// Draw the planet mesh first
	this->mesh.draw();
	// Draw moon
	//this->moonMesh.draw();
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

