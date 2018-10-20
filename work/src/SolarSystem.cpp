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
        CGRA_SRCDIR "/res/shaders/simple.fs.glsl");

    // Create a view matrix that positions the camera
    // 10 units behind the object
    glm::mat4 viewMatrix(1);
    viewMatrix[3] = glm::vec4(0, 0, -10, 1);
    m_program.setViewMatrix(viewMatrix);
	glm::vec3 rotation(1.0f, 1.0f, 0.0f);
	m_rotationMatrix = glm::mat4(1.0f);// glm::rotate(glm::mat4(1.0f), 45.0f, glm::vec3(rotation[0], rotation[1], rotation[2]));
	
	// Create the sun
	generateSun();
	// Setup Basic Planet Info
	generatePlanetSpots();
	generateSystem();
	//planets.push_back(Planet());
}

void SolarSystem::generateSun() {
	this->sun = Planet();
	this->sun.name = "The Sun";
	this->sun.location = glm::vec3(0.0f);
	this->sun.scale = glm::vec3(0.4f);
	this->sun.rotationSpeed = 2.0f;
	// Not going to subdivide 
	std::vector<std::vector<double>> sunColors;
	for (int i = 0; i < 80; i++) {
		//sunColors.push_back({255, ((double) rand() % 255 + 1) }, 0); // Shift the green value for some variation
	}
	this->sun.vertColours = sunColors;
}

void SolarSystem::generateSystem() {
	planets.clear();
	std::vector<PlanetInfo> temp = this->planetSpots;
	// Generate Planets
	for (int i = 0; i < this->numberOfPlanets; i++) {
		int spot = rand() % temp.size();
		PlanetInfo pi = temp.at(spot);
		temp.erase(temp.begin() + spot);
		Planet p = Planet(pi, i);
		p.name = std::to_string(i);
		planets.push_back(p);
	}
}

void SolarSystem::generatePlanetSpots() {
	//std::vector<std::vector<double>> cs1;
	// First Spot: Closest to the sun
	this->planetSpots.push_back(
		generatePlanetInfo(glm::vec3(5.0f, 0.0f, 0.0f), 1.0f,
		{
			{ 1.0f, 0.0f , 0.0f}, {1.0f, 0.0f , 0.0f}, {1.0f, 0.0f , 0.0f}, {1.0f, 0.0f , 0.0f}, {1.0f, 0.0f , 0.0f}
		}
	));
	// Second Spot
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(10.0f, 0.0f, 0.0f), 2.0f,
		{
			{ 1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}
		}
	));
	// Third
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(15.0f, 0.0f, 0.0f), 3.0f,
		{
			{ 1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}
		}
	));
	// Fourth: "Goldy locks zone"
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(20.0f, 0.0f, 0.0f), 4.0f,
		{
			{ 1.0f, 1.0f , 1.0f}, { 1.0f, 1.0f , 1.0f}, { 1.0f, 1.0f , 1.0f}, { 1.0f, 1.0f , 1.0f}, { 1.0f, 1.0f , 1.0f}
		}
	));
	// Fifth
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(25.0f, 0.0f, 0.0f), 5.0f,
		{
			{ 0.8f, 0.8f , 0.8f}, { 0.8f, 0.8f , 0.8f}, { 0.8f, 0.8f , 0.8f}, { 0.8f, 0.8f , 0.8f}, { 0.8f, 0.8f , 0.8f}
		}
	));
	// Sixth
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(30.0f, 0.0f, 0.0f), 6.0f,
		{
			{ 1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}
		}
	));
	// Seventh
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(35.0f, 0.0f, 0.0f), 7.0f,
		{
			{ 1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}, {1.0f, 0.2f , 0.0f}
		}
	));
	// Eighth
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(40.0f, 0.0f, 0.0f), 8.0f,
		{
			{ 0.0f, 0.2f , 1.0f}, {0.0f, 0.2f , 1.0f}, {0.0f, 0.2f , 1.0f}, {0.0f, 0.2f , 1.0f}, {0.0f, 0.2f , 1.0f}
		}
	));
	// Nineth : Furthest away
	this->planetSpots.push_back(generatePlanetInfo(glm::vec3(45.0f, 0.0f, 0.0f), 9.0f,
		{
			{ 0.0f, 0.0f , 1.0f}, {0.0f, 0.0f , 1.0f}, {0.0f, 0.0f , 1.0f}, {0.0f, 0.0f , 1.0f}, {0.0f, 0.0f , 1.0f}
		}
	));
}

/* 
* Generates Planet Info variable, and makes some minor variation tweaks, such as:
* - Changing the starting pos
* - 
*/
PlanetInfo SolarSystem::generatePlanetInfo(glm::vec3 pos, float rs, std::vector<std::vector<double>> cs1) {
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


void SolarSystem::drawScene() {
	// Calculate the aspect ratio of the viewport;
	float aspectRatio = m_viewportSize.x / m_viewportSize.y;
	// Calculate the projection matrix with a field-of-view of 45 degrees
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	// Set the projection matrix
	m_program.setProjectionMatrix(projectionMatrix);

	// Caculate View Matrix depenfding on the mode we are in
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
		// Setup colors
		//GLfloat g_color_buffer_data[5500 * 3 * 3];
		//for (int i = index; i < 5500*3; i++) {
		//	if (i == index+p.vertColours.size()) {
		//		index = i;
		//		break;
		//	}
		//	g_color_buffer_data[i * 3 + 0] = p.vertColours.at(i - index)[0];
		//	g_color_buffer_data[i * 3 + 1] = p.vertColours.at(i - index)[1];
		//	g_color_buffer_data[i * 3 + 2] = p.vertColours.at(i - index)[2];
		//}
		//// Color
		//GLuint colorbuffer;
		//glGenBuffers(1, &colorbuffer);
		//glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
		//// 2nd attribute buffer : colors
		//glEnableVertexAttribArray(2);
		//glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Move the planet and rotate it
		modelTransform = glm::rotate(m_rotationMatrix, (playingRotation) ? ((float)glfwGetTime() / p.rotationSpeed) : 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//modelTransform = glm::rotate(m_rotationMatrix * glm::mat4(1.0f), (float)glfwGetTime() / p.rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
		// Translate the actual mesh
		modelTransform = glm::translate(modelTransform, p.location);
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

		if (ImGui::Checkbox("Cinematic Camera", &cinematicCam)) { // Rotates around the scene
			freeCam = false;
		}

		if (ImGui::Button("Generate New System")) {
			this->generateSystem();
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

		if (ImGui::Checkbox("Random Noise", &this->planets.at(this->currentPlanet).random)) { // Enable Random Noise
			this->planets.at(this->currentPlanet).perlin = false;
			this->planets.at(this->currentPlanet).simplex = false;
		}

		if (ImGui::Checkbox("Perlin Noise", &this->planets.at(this->currentPlanet).perlin)) { // Use Perlin Noise
			this->planets.at(this->currentPlanet).random = false;
			this->planets.at(this->currentPlanet).simplex = false;
		}

		if (ImGui::Checkbox("Simplex Camera", &this->planets.at(this->currentPlanet).simplex)) { // Rotates around the scene
			this->planets.at(this->currentPlanet).perlin = false;
			this->planets.at(this->currentPlanet).random = false;
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
	}

}

void SolarSystem::onScroll(double xoffset, double yoffset) {
    // `(void)foo` suppresses unused variable warnings
    (void)xoffset;
    (void)yoffset;
}