#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "opengl.hpp"
#include "imgui.h"

#include "cgra/matrix.hpp"
#include "cgra/wavefront.hpp"

#include "ex1.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

void Application::init() {
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
	m_rotationMatrix = glm::rotate(glm::mat4(1.0f), 45.0f, glm::vec3(rotation[0], rotation[1], rotation[2]));

    // Create the cube mesh
    createCube();
}

void Application::createCube() {

	cgra::Matrix<double> vertices(36, 3);
	cgra::Matrix<unsigned int> triangles(12, 3);

	vertices.setRow(0, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(1, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(2, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(3, {  1.0f,  1.0f, -1.0f });
	vertices.setRow(4, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(5, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(6, {  1.0f, -1.0f,  1.0f });
	vertices.setRow(7, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(8, {  1.0f, -1.0f, -1.0f });
	vertices.setRow(9, {  1.0f,  1.0f, -1.0f });
	vertices.setRow(10, {  1.0f, -1.0f, -1.0f });
	vertices.setRow(11, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(12, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(13, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(14, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(15, {  1.0f, -1.0f,  1.0f });
	vertices.setRow(16, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(17, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(18, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(19, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(20, {  1.0f, -1.0f,  1.0f });
	vertices.setRow(21, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(22, {  1.0f, -1.0f, -1.0f });
	vertices.setRow(23, {  1.0f,  1.0f, -1.0f });
	vertices.setRow(24, {  1.0f, -1.0f, -1.0f });
	vertices.setRow(25, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(26, {  1.0f, -1.0f,  1.0f });
	vertices.setRow(27, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(28, {  1.0f,  1.0f, -1.0f });
	vertices.setRow(29, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(30, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(31, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(32, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(33, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(34, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(35, {  1.0f, -1.0f,  1.0f });

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

    m_mesh.setData(vertices, triangles);
}

void Application::sphere_latlong() {
	
	/************************************************************
	*
	* Form triangles in order to generate a sphere primitive. 
	* You need to specify discrete steps in both the elevation and azimuthal angles.
	* Let the number of divisions for each angle be integers specified in the ImGui input controls 
	* for this primitive.
	*
	************************************************************/

}
void Application::sphere_from_cube() {
	
	/************************************************************
	*
	* Start with a cube and divide each face into n by n squares and then moving 
	* the new vertices so that they lie on the sphere. Let n be an integer specified in 
	* the ImGui input control, i.e. if n is 1 your sphere is approximated as a cube.
	*
	************************************************************/
}


void Application::drawScene() {
    // Calculate the aspect ratio of the viewport;
    // width / height
    float aspectRatio = m_viewportSize.x / m_viewportSize.y;
    // Calculate the projection matrix with a field-of-view of 45 degrees
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // Set the projection matrix
    m_program.setProjectionMatrix(projectionMatrix);
	glm::mat4 modelTransform = m_rotationMatrix * glm::mat4(1.0f);

    /************************************************************
     *                                                          *
     * Use `m_translation`, `m_scale`, and `m_rotationMatrix`   *
     * to create the  `modelTransform` matrix.                  *
     * The following glm functions will be useful:              *
     *    `glm::translate`                                      *
     *    `glm::scale`                                          *
     ************************************************************/

    m_program.setModelMatrix(modelTransform);

    // Draw the mesh
    m_mesh.draw();
}

void Application::doGUI() {
    ImGui::SetNextWindowSize(ImVec2(450, 450), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Shapes");

    // Example for rotation, use glm to create a a rotation
    // matrix from this vector
    static glm::vec3 rotation(1.0f, 1.0f, 0.0f);
    if (ImGui::InputFloat3("Rotation", &rotation[0])) {
        // This block is executed if the input changes
		m_rotationMatrix = glm::rotate(glm::mat4(1.0f), 45.0f, glm::vec3(rotation[0], rotation[1], rotation[2]));
    }

	static int devisions;
	if (ImGui::InputInt("Number of divisions", &devisions)) {
		// This block is executed if the input changes
		
	}

	static bool sphere_latlong;
	if (ImGui::Checkbox("Use sphere_latlong", &sphere_latlong)) {
			// This block is executed if the input changes

	}

    ImGui::End();
}


// Input Handlers

void Application::onMouseButton(int button, int action, int) {
    if (button >=0 && button < 3) {
        // Set the 'down' state for the appropriate mouse button
        m_mouseButtonDown[button] = action == GLFW_PRESS;
    }
}

void Application::onCursorPos(double xpos, double ypos) {

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

void Application::onKey(int key, int scancode, int action, int mods) {
    // `(void)foo` suppresses unused variable warnings
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
}

void Application::onScroll(double xoffset, double yoffset) {
    // `(void)foo` suppresses unused variable warnings
    (void)xoffset;
    (void)yoffset;
}
