#pragma once

#include "opengl.hpp"
#include <vector>
#include "glm/glm.hpp"
#include <string>
namespace cgra {

    class Program {
        // The OpenGL object representing the program
        GLuint m_program;
		unsigned int greenLeafTexture;

        Program(GLuint prog) : m_program(prog) { }
    public:
        Program() : m_program(0) { }

        // Load the program from two files, a vertex shader and
        // a fragment shader.
        static Program load_program(const char *vertex_shader_file,
                                    const char *fragment_shader_file);

        // Tells OpenGL to use this shader program
        void use();

		// Sets colour
		void setColour(const glm::vec3 &vec);

        // Sets the model matrix for the program.
        // This controls the position, orientation and
        // scale of the model.
        void setModelMatrix(const glm::mat4 &);

        // Sets the view matrix for the program.
        // This controls the position and orientation
        // of the camera.
        void setViewMatrix(const glm::mat4 &);
        // Sets the projection matrix for the program.
        // This specifies how to convert from 3D coordinates to
        // 2D coordinates
        void setProjectionMatrix(const glm::mat4 &);

		// Sets the beta variable that controls the level of fog
		void setBeta(const float &);

		// Sets the position of the viewer. Used for shading calculation
		void setViewerPosition(const glm::vec3 &);

		void specifyLeafTexture(std::string filename);



		void setUpLeafBillboard(glm::mat4 ViewProjectionMatrix, glm::mat4 viewMatrix);

		void buildAirlightData(const char *filename, std::vector<GLubyte> &textureData, int uResolution, int vResolution);

		GLuint getProgram() {
			return m_program;
		}
    };
}
