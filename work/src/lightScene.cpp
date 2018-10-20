#include "lightScene.hpp";
#include <stdio.h>

LightScene::LightScene(cgra::Program shader) {
	m_program = shader;
}

void LightScene::init() {

	// Initialize directional light locations
	m_dirLightLocation.color = glGetUniformLocation(m_program.getProgram(), "directionalLight.base.color");
	m_dirLightLocation.intensity = glGetUniformLocation(m_program.getProgram(), "directionalLight.base.intensity");
	m_dirLightLocation.direction = glGetUniformLocation(m_program.getProgram(), "directionalLight.base.direction");

	// Initialize point light locations
	for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
		char uniformName[128];

		sprintf(uniformName, "pointLights[%d].base.color", i);
		m_pointLightsLocation[i].color = glGetUniformLocation(m_program.getProgram(), uniformName);

		sprintf(uniformName, "pointLights[%d].base.intensity", i);
		m_pointLightsLocation[i].intensity = glGetUniformLocation(m_program.getProgram(), uniformName);

		sprintf(uniformName, "pointLights[%d].base.position", i);
		m_pointLightsLocation[i].position = glGetUniformLocation(m_program.getProgram(), uniformName);
	}
}

void LightScene::setPointLights(unsigned int numLights, const PointLight * pLights) {

	// Set uniform to designate number of lights
	glUniform1i(m_numPointLightsLocation, numLights);

	// Set uniform light attributes for each point light.
	for (unsigned int i = 0; i < numLights; i++) {
		glUniform3f(m_pointLightsLocation[i].color, pLights[i].color.r, pLights[i].color.g, pLights[i].color.b);
		glUniform1f(m_pointLightsLocation[i].intensity, pLights[i].intensity);
		glUniform3f(m_pointLightsLocation[i].position, pLights[i].position.x, pLights[i].position.y, pLights[i].position.z);
	}
}

void LightScene::setDirectionalLight(const DirectionalLight& light) {
	glUniform3f(m_dirLightLocation.color, light.color.r, light.color.g, light.color.b);
	glUniform1f(m_dirLightLocation.intensity, light.intensity);
	glm::vec3 direction = glm::normalize(light.direction);
	glUniform3f(m_dirLightLocation.direction, direction.x, direction.y, direction.z);
}