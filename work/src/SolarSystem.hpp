#pragma once

#include "cgra/mesh.hpp"
#include "cgra/shader.hpp"
#include <string>

#include "glm/glm.hpp"

#include "Planet.hpp"

class SolarSystem {
public:
    // The window object managed by GLFW
    GLFWwindow *m_window;

    // The shader program used for drawing
    cgra::Program m_program;
    // The mesh data
    cgra::Mesh m_mesh;

    // The current size of the viewport
    glm::vec2 m_viewportSize;
    // The current mouse position
    glm::vec2 m_mousePosition;

    // The translation of the mesh as a vec3
    glm::vec3 m_translation = glm::vec3(0);
    // The scale of the mesh
    float m_scale = 1.0f;

    // A 4x4 matrix representing the rotation of the
    // mesh
    glm::mat4 m_rotationMatrix;

    // Whether or not the left, middle or right buttons are down.
    bool m_mouseButtonDown[3];

	Planet sun;
	std::vector<Planet> planets;
	std::vector<PlanetInfo> planetSpots;	
	int numberOfPlanets = 1;

	// Camera Controls
	bool cinematicCam = false;
	bool freeCam = false;

	glm::vec3 position = glm::vec3(0, 0, 10);
	glm::vec3 right = glm::vec3(1.0f);
	glm::vec3 direction = glm::vec3(1.0f);
	glm::vec3 up = glm::vec3(1.0f);
	// horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;

	float movementSpeed = 0.05f;
	float mouseSpeed = 0.00009f;
	float deltaTime = 0.0f;
	float lastTime = 0.0f;

	bool changeScreen;
	int screenNum = 0; // 0 = System Screen, 1 = All planet Info, 2 == Single Planet Info
	int numberOfTris = 0;
	int currentPlanet = 0;
	int subs = 1;

	// 
	bool playingRotation = false;

	// Interaction
	bool wasLeftMouseDown = false;

	SolarSystem(GLFWwindow *win)
        : m_window(win),
          m_viewportSize(1, 1), m_mousePosition(0, 0),
          m_translation(0), m_scale(1), m_rotationMatrix(1) {
        m_mouseButtonDown[0] = false;
        m_mouseButtonDown[1] = false;
        m_mouseButtonDown[2] = false;
    }

    void setWindowSize(int width, int height) {
        m_viewportSize.x = float(width);
        m_viewportSize.y = float(height);
    }

    void init();

	void generateSun();
	void generateSystem();
	void generatePlanetSpots();
	PlanetInfo generatePlanetInfo(glm::vec3 pos, float rs, std::vector<std::vector<double>> cs1);

    void drawScene();
    void doGUI();

    void onKey(int key, int scancode, int action, int mods);

    void onMouseButton(int button, int action, int mods);

    void onCursorPos(double xpos, double ypos);

    void onScroll(double xoffset, double yoffset);
};
