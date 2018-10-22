#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "opengl.hpp"
#include "imgui.h"

#include "cgra/matrix.hpp"
#include "cgra/wavefront.hpp"

#include "SolarSystem.hpp"
#include "Planet.hpp"


#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

void SolarSystem::init() {
    // Load the shader program
    // The use of CGRA_SRCDIR "/path/to/shader" is so you don't
    // have to run the program from a specific folder.
    m_program = cgra::Program::load_program(
        CGRA_SRCDIR "/res/shaders/simple.vs.glsl",
        CGRA_SRCDIR "/res/shaders/volume.fs.glsl");

	billBoardShader = cgra::Program::load_program(
		CGRA_SRCDIR "/res/shaders/Billboard.vs.glsl",
		CGRA_SRCDIR "/res/shaders/Billboard.fs.glsl");

	m_lightScene = LightScene(m_program);
	m_lightScene.init();

	generateLights();
	

    // Create a view matrix that positions the camera
    // 10 units behind the object
    viewMatrix[3] = glm::vec4(0, 0, -9, 1);
	m_program.setViewerPosition(glm::vec3(0, 0, -9));
    m_program.setViewMatrix(viewMatrix);
	glm::vec3 rotation(1.0f, 1.0f, 0.0f);
	m_rotationMatrix = glm::mat4(1.0f);// glm::rotate(glm::mat4(1.0f), 45.0f, glm::vec3(rotation[0], rotation[1], rotation[2]));
	
	//TREESTUFF
	generateCylinder();
	billBoardShader.setViewMatrix(viewMatrix);

	ls.readRules(CGRA_SRCDIR "/res/TreeFiles/Basic.txt");
	billBoardShader.specifyLeafTexture();
	for (int i = 0; i < 5; i++) {
		ls.generate();
	}
	// Create the sun
	generateSun();
	// Setup Basic Planet Info
	generatePlanetSpots();
	generateSystem();
	createCube();
	//planets.push_back(Planet());

}

void::SolarSystem::generateLights() {
	PointLight p = PointLight(glm::vec3(1, 0, 0), 1, glm::vec3(10, 10, 10));
	PointLight p2 = PointLight(glm::vec3(0, 1, 0), 1, glm::vec3(-20, -20, -20));
	PointLight p3 = PointLight(glm::vec3(1, 1, 0), 20, glm::vec3(0, 0, 0)); // Sun
	PointLight p4 = PointLight(glm::vec3(0, 1, 1), 20, glm::vec3(10, -10, 9));
	PointLight p5 = PointLight(glm::vec3(1, 1, 1), 20, glm::vec3(-13, 6, 12));
	PointLight p6 = PointLight(glm::vec3(0, .5, .5), 20, glm::vec3(9, 9, -8));
	PointLight pLights[] = { p, p2, p3, p4, p5, p6 };

	DirectionalLight d = DirectionalLight(glm::vec3(0.25, 0.25, -1));

	m_lightScene.setPointLights(6, &pLights[0]);

	m_lightScene.setDirectionalLight(d);
}

void SolarSystem::generateSun() {
	this->sun = Planet();
	this->sun.name = "The Sun";
	this->sun.location = glm::vec3(0.0f);
	this->sun.scale = glm::vec3(0.4f);
	this->sun.rotationSpeed = 2.0f;
}

void SolarSystem::generateSystem() {
	double startTime = glfwGetTime();
	planets.clear();
	std::vector<PlanetInfo> temp = this->planetSpots;
	// Generate Planets
	for (int i = 0; i < this->numberOfPlanets; i++) {
		int spot = rand() % temp.size();
		PlanetInfo pi = temp.at(spot);
		temp.erase(temp.begin() + spot);
		Planet p = Planet(pi, i, this->octaves, this->freq, this->amp, this->subs);
		p.name = std::to_string(i);
		planets.push_back(p);
	}
	double timeTaken = glfwGetTime() - startTime;
	std::cout << "Generated System, Time Taken: " << timeTaken << " Seconds" << std::endl;
}

/*
* Biome Keys:
* 0 = FlatLand (Grass)
* 1 = Desert
* 2 = Snow
* 3 = Jungle
* 4 = Urban
*/
void SolarSystem::generatePlanetSpots() {
	//std::vector<std::vector<double>> cs1;
	// First Spot: Closest to the sun
	this->planetSpots.push_back(
		generatePlanetInfo(glm::vec3(5.0f, 0.0f, 0.0f), 1.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 1.0f, 0.0f , 0.0f}, {1.0f, 0.0f , 0.0f}, {1.0f, 0.0f , 0.0f}, {1.0f, 0.0f , 0.0f}, {0.5f, 0.5f , 0.5f}
		}
	));
	// Second Spot
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(10.0f, 0.0f, 0.0f), 2.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}
		}
	));
	// Third
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(15.0f, 0.0f, 0.0f), 3.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}
		}
	));
	// Fourth: "Goldy locks zone"
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(20.0f, 0.0f, 0.0f), 4.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 0.0f, 1.0f , 0.0f}, { 237.0f/255.0f, 166.0f / 255.0f , 66.0f / 255.0f}, { 1.0f, 1.0f , 1.0f}, { 0.0f, 0.7f , 0.0f}, { 0.5f, 0.5f , 0.5f}
		}
	));
	// Fifth
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(25.0f, 0.0f, 0.0f), 5.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 0.8f, 0.8f , 0.8f}, { 0.8f, 0.8f , 0.8f}, { 0.8f, 0.8f , 0.8f}, { 0.8f, 0.8f , 0.8f}, { 0.8f, 0.8f , 0.8f}
		}
	));
	// Sixth
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(30.0f, 0.0f, 0.0f), 6.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}
		}
	));
	// Seventh
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(35.0f, 0.0f, 0.0f), 7.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}
		}
	));
	// Eighth
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(40.0f, 0.0f, 0.0f), 8.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 0.0f, 0.2f , 1.0f}, {0.0f, 0.2f , 1.0f}, {0.0f, 0.2f , 1.0f}, {0.0f, 0.2f , 1.0f}, {0.0f, 0.2f , 1.0f}
		}
	));
	// Nineth : Furthest away
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(45.0f, 0.0f, 0.0f), 9.0f,
		{
			{ 0.0f, 1.0f , 0.0f}, { 0.0f, 0.0f , 1.0f}, {0.0f, 0.0f , 1.0f}, {0.0f, 0.0f , 1.0f}, {0.0f, 0.0f , 1.0f}, {0.0f, 0.0f , 1.0f}
		}
	));
}

void::SolarSystem::generateTree(mat4 transMat, vec3 startPos, float length, float trunkSize, int &index) {
	float  angle = ls.angle;
	float tsize = trunkSize;

	for (; index < ls.currentTree.length(); index++) {
		char c = ls.currentTree.at(index);
		if (c == 'F') {
			glm::vec3 midPoint(0, length, 0);
			mat4 cyMat = glm::translate(transMat, midPoint);
			cyMat = glm::scale(cyMat, vec3(tsize*0.2, length, tsize*0.2)) ; // translate tree down so we cna see all of it

			m_program.setModelMatrix(cyMat);
			//m_program.setColour(vec3(1, 0.8, 0.6));
			m_cylinder.draw();

			transMat = translate(transMat, vec3(0, length, 0));
		}
		else if (c == 'f') {
			transMat = translate(transMat, vec3(0, length, 0));
		}
		else if (c == '[') {
			index++;
			generateTree(transMat, startPos, length, tsize, index);
		}
		else if (c == ']') {
			return;
		}
		else if (c == '+') {
			transMat = rotate(transMat, angle, vec3(0, 0, 1));
		}
		else if (c == '-') {
			transMat = rotate(transMat, -angle, vec3(0, 0, 1));
		}
		else if (c == '&') {
			transMat = rotate(transMat, angle, vec3(1, 0, 0));
		}
		else if (c == '^') {
			transMat = rotate(transMat, -angle, vec3(1, 0, 0));
		}
		else if (c == '\\' || c == '<') {
			transMat = rotate(transMat, angle, vec3(0, 1, 0));
		}
		else if (c == '/' || c == '>') {
			transMat = rotate(transMat, -angle, vec3(0, 1, 0));
		}
		else if (c == '|') {
			transMat = rotate(transMat, radians(180.f), vec3(0, 0, 1));
		}
		else if (c == '!') {
			//transMat = glm::scale(transMat, vec3(0.8));

			//TODO trunksize
			tsize -= (0.15* trunkSize);
		}
		else if (c == '\'') {
			//TODO increment color index
		}
		else if (c == 'T') {
			glm::vec3 midPoint(0, length, 0);
			mat4 cyMat = glm::translate(transMat, midPoint);

			glm::vec3 scale(1);
			mat4 scaleMat = glm::scale(cyMat, scale);

			scaleMat = translate(scaleMat, vec3(0, -0.5, 0));
			mat4 td = scaleMat;

			m_program.setModelMatrix(td);

			m_program.setModelMatrix(td);
			billBoardShader.setModelMatrix(td);
			drawLeaf();
		}
	}
}

void SolarSystem::generateCylinder() {
	int divisions = 5;

	std::vector<unsigned int> indices;
	cgra::Matrix<double> vertices((divisions + 1) * (divisions + 1), 3);
	std::vector<glm::vec3> vertColours;
	int count = 0;

	for (int i = 0; i <= divisions; ++i) {
		float V = i / (float)divisions;
		float azi = V * glm::pi<float>();

		for (int j = 0; j <= divisions; ++j) {

			float U = j / (float)divisions;
			float ele = U * (glm::pi <float>() * 2);

			float x = cos(ele) * sin(azi);
			float y = cos(azi);
			if (i >= (divisions / 2) + 1) {
				y = cos(azi);
			}
			float z = sin(ele) * sin(azi);
			vertColours.push_back(vec3(1, 0.8, 0.6));
			vertices.setRow(count++, { x,y,z });
		}
	}

	for (int i = 0; i < ((divisions + 1) * (divisions + 1) * 2); i++) {

		if (i + divisions + 1 >= vertices.numRows()) {
			continue;
		}

		//Bot corner
		indices.push_back(i);
		indices.push_back(i + divisions + 1);
		indices.push_back(i + divisions);

		//Top corner
		indices.push_back(i + divisions + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}

	cgra::Matrix<unsigned int> triangles(indices.size() / 3, 3);
	count = 0;
	for (int i = 0; i < indices.size(); i += 3) {
		triangles.setRow(count++, { indices.at(i), indices.at(i + 1), indices.at(i + 2) });
	}

	m_cylinder.setData(vertices, triangles, vertColours);
}



void SolarSystem::drawLeaf() {
	float aspectRatio = m_viewportSize.x / m_viewportSize.y;

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 200.0f);

	glm::mat4 ViewProjectionMatrix = projectionMatrix * viewMatrix;

	//billBoardShader.setProjectionMatrix(projectionMatrix);

	billBoardShader.setUpLeafBillboard(ViewProjectionMatrix, viewMatrix);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	billBoardShader.use();

	// Draw the billboard !
	// This draws a triangle_strip which looks like a quad.
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(2);

}

mat4 SolarSystem::createTreeTransMatrix(vec3 startPoint) {
	glm::vec3 fdir = glm::vec3(0, 1.f, 0);
	vec3 vPos = startPoint;
	float angle = glm::acos(glm::dot(fdir, vPos));
	glm::vec3 newdir = glm::cross(fdir, vPos);
	mat4 td = glm::rotate(mat4(1), angle , newdir);
	td = translate(td, vec3(0, -vPos.y, 0));
	return td;
}

/* 
* Generates Planet Info variable, and makes some minor variation tweaks, such as:
* - Changing the starting pos
* - 
*/
PlanetInfo SolarSystem::generatePlanetInfo(glm::vec3 pos, float rs, std::vector<glm::vec3> cs1) {
	PlanetInfo pi = PlanetInfo();
	// Decide its position
	if (rand() % 10 >= 5) {
		pi.location = -pos;
	} else {
		pi.location = pos;
	}
	pi.colorSet1 = cs1;
	pi.rotationSpeed = rs;
	return pi;
}

void SolarSystem::createCube() {

	cgra::Matrix<double> vertices(36, 3);
	cgra::Matrix<unsigned int> triangles(12, 3);

	vertices.setRow(0, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(1, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(2, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(3, { 1.0f,  1.0f, -1.0f });
	vertices.setRow(4, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(5, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(6, { 1.0f, -1.0f,  1.0f });
	vertices.setRow(7, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(8, { 1.0f, -1.0f, -1.0f });
	vertices.setRow(9, { 1.0f,  1.0f, -1.0f });
	vertices.setRow(10, { 1.0f, -1.0f, -1.0f });
	vertices.setRow(11, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(12, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(13, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(14, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(15, { 1.0f, -1.0f,  1.0f });
	vertices.setRow(16, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(17, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(18, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(19, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(20, { 1.0f, -1.0f,  1.0f });
	vertices.setRow(21, { 1.0f,  1.0f,  1.0f });
	vertices.setRow(22, { 1.0f, -1.0f, -1.0f });
	vertices.setRow(23, { 1.0f,  1.0f, -1.0f });
	vertices.setRow(24, { 1.0f, -1.0f, -1.0f });
	vertices.setRow(25, { 1.0f,  1.0f,  1.0f });
	vertices.setRow(26, { 1.0f, -1.0f,  1.0f });
	vertices.setRow(27, { 1.0f,  1.0f,  1.0f });
	vertices.setRow(28, { 1.0f,  1.0f, -1.0f });
	vertices.setRow(29, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(30, { 1.0f,  1.0f,  1.0f });
	vertices.setRow(31, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(32, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(33, { 1.0f,  1.0f,  1.0f });
	vertices.setRow(34, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(35, { 1.0f, -1.0f,  1.0f });

	triangles.setRow(0, { 0, 1, 2 });
	triangles.setRow(1, { 3, 4, 5 });
	triangles.setRow(2, { 6, 7, 8 });
	triangles.setRow(3, { 9, 10, 11 });
	triangles.setRow(4, { 12, 13, 14 });
	triangles.setRow(5, { 15, 16, 17 });
	triangles.setRow(6, { 18, 19, 20 });
	triangles.setRow(7, { 21, 22, 23 });
	triangles.setRow(8, { 24, 25, 26 });
	triangles.setRow(9, { 27, 28, 29 });
	triangles.setRow(10, { 30, 31, 32 });
	triangles.setRow(11, { 33, 34, 35 });

	std::vector<glm::vec3> vertColours;
	for (int i = 0; i < 36; i++) {
		vertColours.push_back(vec3(0.0f));
	}

	m_cube.setData(vertices, triangles, vertColours);
}

void SolarSystem::drawBoundingBox() {
	GLuint airlightOnly;
	glUniform1i(glGetUniformLocation(m_program.getProgram(), "onlyPointLights"), 1);
	//m_program.setColour(glm::vec3(0, 0, 0));
	// Bottom
	glm::mat4 modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, glm::vec3(0, -50, 0));
	modelTransform = glm::scale(modelTransform, glm::vec3(50, 1, 50));
	
	m_program.setModelMatrix(modelTransform);
	m_cube.draw();

	//Top
	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, glm::vec3(0, 50, 0));
	modelTransform = glm::scale(modelTransform, glm::vec3(50, 1, 50));
	m_program.setModelMatrix(modelTransform);
	m_cube.draw();

	//Front
	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, glm::vec3(0, 0, -50));
	modelTransform = glm::scale(modelTransform, glm::vec3(50, 50, 1));
	m_program.setModelMatrix(modelTransform);
	m_cube.draw();

	//Back
	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, glm::vec3(0, 0, 50));
	modelTransform = glm::scale(modelTransform, glm::vec3(50, 50, 1));
	m_program.setModelMatrix(modelTransform);
	m_cube.draw();

	//Left
	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, glm::vec3(-50, 0, 0));
	modelTransform = glm::scale(modelTransform, glm::vec3(1, 50, 50));
	m_program.setModelMatrix(modelTransform);
	m_cube.draw();

	//Right
	modelTransform = glm::mat4(1.0f);
	modelTransform = glm::translate(modelTransform, glm::vec3(50, 0, 0));
	modelTransform = glm::scale(modelTransform, glm::vec3(1, 50, 50));
	m_program.setModelMatrix(modelTransform);
	m_cube.draw();
	glUniform1i(glGetUniformLocation(m_program.getProgram(), "onlyPointLights"), 0);
}


void SolarSystem::drawScene() {
	// Calculate the aspect ratio of the viewport;
	float aspectRatio = m_viewportSize.x / m_viewportSize.y;
	// Calculate the projection matrix with a field-of-view of 45 degrees
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	// Set the projection matrix
	m_program.setProjectionMatrix(projectionMatrix);
	billBoardShader.setProjectionMatrix(projectionMatrix);

	// Caculate View Matrix depending on the mode we are in
	glm::mat4 viewMatrix(1);
	if (cinematicCam) {
		float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		viewMatrix = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	
	if (freeCam) { // Does the user want to move the camera about
		double currentTime = glfwGetTime();
		deltaTime = float(currentTime - lastTime);
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		glfwSetCursorPos(m_window, m_viewportSize.x / 2, m_viewportSize.y / 2);
		horizontalAngle += mouseSpeed * deltaTime * float(m_viewportSize.x / 2 - xpos);
		verticalAngle += mouseSpeed * deltaTime * float(m_viewportSize.y / 2 - ypos);
	}
	direction = glm::vec3(cos(verticalAngle) * sin(horizontalAngle),sin(verticalAngle),	cos(verticalAngle) * cos(horizontalAngle));
	right = glm::vec3( sin(horizontalAngle - 3.14f / 2.0f),	0, cos(horizontalAngle - 3.14f / 2.0f)	);

	// Perpendicular to both direction and right camera views
	up = glm::cross(right, direction);
	
	viewMatrix = glm::lookAt(position, position + direction, up);	
	m_program.setViewMatrix(viewMatrix);

	// Draw the sun (code to rotate object = ((float)glfwGetTime() / p.rotationSpeed))
	glm::mat4 modelTransform = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	// Translate the actual mesh
	modelTransform = glm::translate(modelTransform, sun.location);
	// Scale the mesh
	modelTransform = glm::scale(modelTransform, glm::vec3(1.3f));
	// Draw the mesh
	m_program.setModelMatrix(modelTransform);
	sun.draw();	
	
	// Draw each planet
	for (Planet p : planets) {	
		// Move the planet and rotate it
		modelTransform = glm::rotate(m_rotationMatrix, (playingRotation) ? ((float)glfwGetTime() / p.rotationSpeed) : 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	
		//modelTransform = glm::rotate(m_rotationMatrix * glm::mat4(1.0f), (float)glfwGetTime() / p.rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
		// Translate the actual mesh

		modelTransform = glm::translate(modelTransform, p.location);
	

		mat4 td = createTreeTransMatrix(p.originalVerticies.at(4));
		mat4 td1 = createTreeTransMatrix(p.originalVerticies.at(22));
		mat4 td2 = createTreeTransMatrix(p.originalVerticies.at(15));

		int h = 0;
		generateTree(modelTransform *td, vec3(0), 0.05, 0.05, h);
	/*	generateTree(modelTransform *td1, vec3(0), 0.05, 0.3, h = 0);
		generateTree(modelTransform *td2, vec3(0), 0.05, 0.3, h = 0);*/

		// Scale the mesh
		modelTransform = glm::scale(modelTransform, p.scale);
		// Draw the mesh
		m_program.setModelMatrix(modelTransform);
		p.draw();
		if (p.hasMoon) {
			// Move the planet and rotate it
			modelTransform = glm::rotate(m_rotationMatrix, (playingRotation) ? ((float)glfwGetTime() / p.rotationSpeed) : 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotates with planet
			modelTransform = glm::rotate(modelTransform, (playingRotation) ? ((float)glfwGetTime() / p.rotationSpeed) : 0.0f, p.location); // Rotates around planet
			// Translate the actual mesh
			modelTransform = glm::translate(modelTransform, glm::vec3(p.location.x, p.location.y+0.5f, p.location.z+1.25f));			
			// Scale the mesh
			modelTransform = glm::scale(modelTransform, glm::vec3(0.2f));
			// Draw the mesh
			m_program.setModelMatrix(modelTransform);
			p.moonMesh.draw();
		}
	}

	// Draw Bounding Box
	drawBoundingBox();
}

void SolarSystem::doGUI() {
	if (this->screenNum == 0) {
		ImGui::SetNextWindowSize(ImVec2(450, 150), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Solar System Settings"); // Used for planet Gen UI
		ImGui::Text("Solar System Information");

		int pt = this->sun.originalTriangles.size();
		for (int i = 0; i < this->planets.size(); i++) {
			pt += this->planets.at(i).originalTriangles.size();
		}
		std::string totalTris = "Total Number of Tris: " + std::to_string(pt);
		ImGui::Text(totalTris.data());

		if (ImGui::InputInt("Number of Planets", &numberOfPlanets)) {
			if (numberOfPlanets < 0) {
				numberOfPlanets = 0;
			}
			if (numberOfPlanets > 9) {
				numberOfPlanets = 9;
			}
		}
		
		if (ImGui::InputInt("Planet Subdivisions", &this->subs)) {
			if (this->subs < 0) {
				this->subs = 0;
			}
			if (this->subs > 3) {
				this->subs = 3;
			}
		}

		if (ImGui::InputInt("Frequency", &this->freq)) {
			if (this->freq < 0) {
				this->freq = 0;
			}
		}
		
		if (ImGui::InputInt("Scale", &this->amp)) {
			if (this->amp < 0) {
				this->amp = 0;
			}
		}
		
		if (ImGui::InputInt("Octaves", &this->octaves)) {
			if (this->octaves < 0) {
				this->octaves = 0;
			}
		}

		if (ImGui::Checkbox("Cinematic Camera", &cinematicCam)) { // Rotates around the scene
			freeCam = false;
		}

		std::string rot;
		if (playingRotation) {
			rot = "Pause";
		}
		else {
			rot = "Play";
		}
		if (ImGui::Button(rot.data())) {
			if (playingRotation) {
				playingRotation = false;
			}
			else {
				playingRotation = true;
			}
		}			

		static float beta;
		if (ImGui::SliderFloat("Beta", &beta, 0.0f, 0.04f, "%.2f")) {
			m_program.setBeta(beta);
		}


		if (ImGui::Button("Generate New System")) {
			this->generateSystem();
		}

		ImGui::End();
	}
	else {
		ImGui::SetNextWindowSize(ImVec2(450, 250), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Planet Settings"); // Used for planet Gen UI
		ImGui::Text("Current Planet Information");
		std::string planetName = "Planet Name: " + this->planets.at(this->currentPlanet).name;
		ImGui::Text(planetName.data());

		std::string planetTris = "Planet Tris: " + std::to_string(this->planets.at(this->currentPlanet).originalTriangles.size());
		ImGui::Text(planetTris.data());

		if (ImGui::Checkbox("Perlin Noise", &this->planets.at(this->currentPlanet).perlin)) { // Use Perlin Noise
			this->planets.at(this->currentPlanet).simplex = false;
		}

		if (ImGui::Checkbox("Simplex Camera", &this->planets.at(this->currentPlanet).simplex)) { // Rotates around the scene
			this->planets.at(this->currentPlanet).perlin = false;
		}

		if (ImGui::InputInt("Planet Subdivisions", &this->planets.at(this->currentPlanet).subdivisions)) {
			if (this->planets.at(this->currentPlanet).subdivisions < 0) {
				this->planets.at(this->currentPlanet).subdivisions = 0;
			}
			if (this->planets.at(this->currentPlanet).subdivisions > 3) {
				this->planets.at(this->currentPlanet).subdivisions = 3;
			}
		}

		if (ImGui::InputFloat("Frequency", &this->planets.at(this->currentPlanet).frequency)) {
			if (this->planets.at(this->currentPlanet).frequency < 0) {
				this->planets.at(this->currentPlanet).frequency = 0;
			}
			// Regenerate 
			this->planets.at(this->currentPlanet).generateTerrain();
		}

		if (ImGui::InputFloat("Scale", &this->planets.at(this->currentPlanet).amplitude)) {
			if (this->planets.at(this->currentPlanet).amplitude < 0) {
				this->planets.at(this->currentPlanet).amplitude = 0;
			}
			// Regenerate 
			this->planets.at(this->currentPlanet).generateTerrain();
		}

		if (ImGui::InputInt("Octaves", &this->planets.at(this->currentPlanet).octaves)) {
			if (this->planets.at(this->currentPlanet).octaves < 0) {
				this->planets.at(this->currentPlanet).octaves = 0;
			}
			// Regenerate 
			this->planets.at(this->currentPlanet).generateTerrain();
		}

		if (ImGui::Button("Regenerate Planet")) {
			this->planets.at(this->currentPlanet).generatePlanet();
		}

		if (ImGui::Checkbox("Cinematic Camera", &cinematicCam)) { // Rotates around the scene
			freeCam = false;
		}

		if (ImGui::Button("Reset Camera")) { // Just incase user loses track
			freeCam = false;
			glm::vec3 position = glm::vec3(0, 0, 10);
			glm::vec3 right = glm::vec3(1.0f);
			glm::vec3 direction = glm::vec3(1.0f);
			glm::vec3 up = glm::vec3(1.0f);
			float horizontalAngle = 3.14f; // horizontal angle : toward -Z			
			float verticalAngle = 0.0f; // vertical angle : 0, look at the horizon
		}

		if (ImGui::Button("Next Planet")) {
			this->currentPlanet = (this->currentPlanet + 1) % planets.size();
		}

		if (ImGui::Button("Previous Planet")) {

		}
		ImGui::End();
	}
}


// Input Handlers

void SolarSystem::onMouseButton(int button, int action, int) {
    if (button >=0 && button < 3) {
        // Set the 'down' state for the appropriate mouse button
        m_mouseButtonDown[button] = action == GLFW_PRESS;
    }
}

void SolarSystem::onCursorPos(double xpos, double ypos) {
    // Make a vec2 with the current mouse position
    glm::vec2 currentMousePosition(xpos, ypos);

    // Get the difference from the previous mouse position
    glm::vec2 mousePositionDelta = currentMousePosition - m_mousePosition;

    if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_LEFT]) {

    } else if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_MIDDLE]) {

    } else if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_RIGHT]) {

    }

    // Update the mouse position to the current one
    m_mousePosition = currentMousePosition;
}

void SolarSystem::onKey(int key, int scancode, int action, int mods) {
    // `(void)foo` suppresses unused variable warnings
	if (GLFW_KEY_X == key && action == GLFW_PRESS) {
		this->screenNum = (this->screenNum + 1) % 2;
	}

	// Turn freecam on/off
	if (GLFW_KEY_F == key && action == GLFW_PRESS) {
		cinematicCam = false;
		if (this->freeCam) {
			this->freeCam = false;
		}
		else {
			this->freeCam = true;
		}
	}

    // Free cam controls
	if (freeCam) {
		if (GLFW_KEY_W == key) {
			position += direction * deltaTime * movementSpeed;
		}

		if (GLFW_KEY_A == key) {
			position -= right * deltaTime * movementSpeed;
		}

		if (GLFW_KEY_S == key) {
			position -= direction * deltaTime * movementSpeed;
		}

		if (GLFW_KEY_D == key) {
			position += right * deltaTime * movementSpeed;
		}
		m_program.setViewerPosition(position);
	}

}

void SolarSystem::onScroll(double xoffset, double yoffset) {
    // `(void)foo` suppresses unused variable warnings
    (void)xoffset;
    (void)yoffset;
}