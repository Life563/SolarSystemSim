#include "lightScene.hpp";
#include <stdio.h>

LightScene::LightScene() {}

LightScene::LightScene(cgra::Program shader) {
	m_program = shader;
}

void LightScene::init() {
	m_numPointLightsLocation = glGetUniformLocation(m_program.getProgram(), "numPointLights");

	// Initialize directional light locations
	m_dirLightLocation.color = glGetUniformLocation(m_program.getProgram(), "directionalLight.base.color");
	m_dirLightLocation.intensity = glGetUniformLocation(m_program.getProgram(), "directionalLight.base.intensity");
	m_dirLightLocation.direction = glGetUniformLocation(m_program.getProgram(), "directionalLight.direction");

	// Initialize point light locations
	for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++) {
		char uniformName[128];

		sprintf(uniformName, "pointLights[%d].base.color", i);
		m_pointLightsLocation[i].color = glGetUniformLocation(m_program.getProgram(), uniformName);

		sprintf(uniformName, "pointLights[%d].base.intensity", i);
		m_pointLightsLocation[i].intensity = glGetUniformLocation(m_program.getProgram(), uniformName);

		sprintf(uniformName, "pointLights[%d].position", i);
		m_pointLightsLocation[i].position = glGetUniformLocation(m_program.getProgram(), uniformName);
	}

	// Build Airlight data
	std::vector<GLubyte> airlightTextureData;
	m_program.buildAirlightData(CGRA_SRCDIR "/res/lookups/F.csv", airlightTextureData, 512, 512);
	GLuint airlightTex;
	glGenTextures(1, &airlightTex); // Generates an array of one texture and stores it in tex
	glBindTexture(GL_TEXTURE_2D, airlightTex); // Bind the texture so that any subsequent texture commands will configure the currently bound texture
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, 512, 512);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_RED, GL_UNSIGNED_BYTE, &airlightTextureData[0]);  // Generate the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, airlightTex);
	glUniform1i(glGetUniformLocation(m_program.getProgram(), "airlightLookup"), 0);

	// Build G0 data
	std::vector<GLubyte> G0TextureData;
	m_program.buildAirlightData(CGRA_SRCDIR "/res/lookups/G0.csv", G0TextureData, 64, 64);
	GLuint G0Tex;
	glGenTextures(1, &G0Tex); // Generates an array of one texture and stores it in tex
	glBindTexture(GL_TEXTURE_2D, G0Tex); // Bind the texture so that any subsequent texture commands will configure the currently bound texture
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, 64, 64);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 64, GL_RED, GL_UNSIGNED_BYTE, &G0TextureData[0]);  // Generate the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, G0Tex);
	glUniform1i(glGetUniformLocation(m_program.getProgram(), "G0"), 0);

	// Build G20 data
	std::vector<GLubyte> G20TextureData;
	m_program.buildAirlightData(CGRA_SRCDIR "/res/lookups/G20.csv", G20TextureData, 64, 64);
	GLuint G20Tex;
	glGenTextures(1, &G20Tex); // Generates an array of one texture and stores it in tex
	glBindTexture(GL_TEXTURE_2D, G20Tex); // Bind the texture so that any subsequent texture commands will configure the currently bound texture
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, 64, 64);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 64, GL_RED, GL_UNSIGNED_BYTE, &G20TextureData[0]);  // Generate the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, G20Tex);
	glUniform1i(glGetUniformLocation(m_program.getProgram(), "G20"), 0);

	m_program.setBeta(0.04);
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