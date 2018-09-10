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

	// Create Tetrahedron
	generateTetrahedron();

}

void Planet::generateTetrahedron() {

}

void Planet::generatePlanet() {

}

void Planet::draw() {
	// Draw the planet mesh first
	this->mesh.draw();
	// Draw the items on the planet afterwards
}

//========================================================//
//================= Alans Methods ========================//
//========================================================//

/*
 * Method used to populate the planet. Alans Main method
 */
void Planet::populatePlanet() {

}

