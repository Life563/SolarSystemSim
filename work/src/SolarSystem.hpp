#pragma once

#include "cgra/mesh.hpp"
#include "cgra/shader.hpp"
#include <string>

#include "glm/glm.hpp"

#include "Planet.hpp"
#include "LSystem.hpp"
#include "lightScene.hpp"

using namespace glm;

class SolarSystem {
public:
    // The window object managed by GLFW
    GLFWwindow *m_window;

	LightScene m_lightScene;

    // The shader program used for drawing
    cgra::Program m_program;
	cgra::Program billBoardShader;

    // The mesh data
    cgra::Mesh m_mesh;
	cgra::Mesh m_cylinder;
	cgra::Mesh m_cube;
	cgra::Mesh m_Selector;

    // The current size of the viewport
    glm::vec2 m_viewportSize;
    // The current mouse position
    glm::vec2 m_mousePosition;

// TREE STUFF
	LSystem basicTrees;
	LSystem dessertTrees;
	LSystem snowTrees;
	LSystem jungleTrees;
	LSystem urbanTrees;
	bool m_showTrees = false;
	bool showLeaves = false;
	bool polyLeaves = true;
	int cylinderDiv = 9;
	int treeGenerations = 3;

	cgra::Mesh m_leaf;


    // The translation of the mesh as a vec3
    glm::vec3 m_translation = glm::vec3(0);
    // The scale of the mesh
    float m_scale = 1.0f;

    // A 4x4 matrix representing the rotation of the
    // mesh
    glm::mat4 m_rotationMatrix;

	glm::mat4 viewMatrix;

    // Whether or not the left, middle or right buttons are down.
    bool m_mouseButtonDown[3];

	Planet sun;
	std::vector<Planet> planets;
	std::vector<PlanetInfo> planetSpots;
	int numberOfPlanets = 1;

	// Camera Controls
	bool freeCam = false;

	glm::vec3 position = glm::vec3(0, 0, 10);
	glm::vec3 right = glm::vec3(1.0f);
	glm::vec3 direction = glm::vec3(1.0f);
	glm::vec3 up = glm::vec3(1.0f);

	float horizontalAngle = 3.14f; // horizontal angle : toward -Z
	float verticalAngle = 0.0f; // vertical angle : 0, look at the horizon

	// Movement and mouse
	float movementSpeed = 0.05f;
	float mouseSpeed = 0.00009f;
	float deltaTime = 0.0f;
	float lastTime = 0.0f;

	bool changeScreen;
	bool wire = false;
	int screenNum = 0; // 0 = System Screen, 1 = All planet Info, 2 == Single Planet Info
	int numberOfTris = 0;
	int currentPlanet = 0;
	int subs = 3;

	// Global Noise Functions
	int freq = 3;
	int amp = 6;
	int octaves = 4;

	double timeTaken;

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

	cgra::Mesh createCube();
	void drawBoundingBox();

	void generateSun();
	void generateSystem();
	void generatePlanetSpots();
	void loadObj(const char * filename, cgra::Mesh * mesh);
	void generateCylinder();

	void drawLeaf();

	mat4 createTreeTransMatrix(vec3 startPoint);

	void generateLights();

	void generateTree(LSystem LS, mat4 transMat, vec3 startPos, float length, float trunkSize, int & index);


	PlanetInfo generatePlanetInfo(glm::vec3 pos, float rs, std::vector<glm::vec3> cs1);

    void drawScene();
    void doGUI();

    void onKey(int key, int scancode, int action, int mods);

    void onMouseButton(int button, int action, int mods);

    void onCursorPos(double xpos, double ypos);

    void onScroll(double xoffset, double yoffset);
};
