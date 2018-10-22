#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <stdexcept>
#include "stb_image.h"

#include "shader.hpp"

namespace cgra {

    // Helper function for compiling a single shader stage
    static GLuint compileShader(const char *src, GLenum type);

    // Helper function for reading a file into a string
    static std::string read_all(std::ifstream &in) {
        std::stringstream sstr;
        sstr << in.rdbuf();
        return sstr.str();
    }

    // Helper function for reading a file into a string
    static std::string read_all(const char *filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::ostringstream msgStream;
            msgStream << "Cannot open file '" << filename << "'";
            throw std::runtime_error(msgStream.str());
        }
        return read_all(file);
    }

    Program Program::load_program(const char *vsFile, const char *fsFile) {
        // Read the contents of the vertex and fragment shader files into memory
        std::string vsSrc = read_all(vsFile);
        std::string fsSrc = read_all(fsFile);

        // Compile the shader stages.
        GLuint vs = compileShader(vsSrc.c_str(), GL_VERTEX_SHADER);
        GLuint fs = compileShader(fsSrc.c_str(), GL_FRAGMENT_SHADER);

        // Create a new program object
        GLuint program = glCreateProgram();

        // Attach the shader stages to the program
        glAttachShader(program, vs);
        glAttachShader(program, fs);

        // Set the attribute locations
        // 0 is the position.
        // 1 is the normal.
        glBindAttribLocation(program, 0, "vertPosition");
        glBindAttribLocation(program, 1, "vertexColor");

        // Link the stages together. This must be done
        // after setting the program
        glLinkProgram(program);

        // Check to see that the linking was successful
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE) {
            // Get the length of the error message
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            //The maxLength includes the NULL character
            std::string infoLog(maxLength, 0);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // Print an error message
            std::cerr << "Failed to link program:\n" << infoLog << std::endl;

            //We don't need the shader anymore.
            glDeleteProgram(program);
            glDeleteShader(vs);
            glDeleteShader(fs);

            // Abort the process.
            abort();
        }

        // Link was successful, we can detach the shader stages from the
        // program now
        glDetachShader(program, vs);
        glDetachShader(program, fs);

        // We can also delete the shader objects, since we won't be using
        // them again
        glDeleteShader(vs);
        glDeleteShader(fs);

        auto prog = Program(program);

        // Set default values for the matrices
        prog.setModelMatrix(glm::mat4(1));
        prog.setViewMatrix(glm::mat4(1));
        prog.setProjectionMatrix(glm::mat4(1));

        return prog;
    }

    void Program::use() {
        if (m_program != 0) {
            glUseProgram(m_program);
        }
    }

	void Program::setColour(const glm::vec3 &vec) {
		if (m_program == 0) return;
		use();

		// Get the location of the "modelMat" uniform variable
		GLint colourLoc = glGetUniformLocation(m_program, "ColorVec");
		// Set the value of "colourLoc"
		glUniform3fv(colourLoc, 1, (float*)&vec);
	}

    void Program::setModelMatrix(const glm::mat4 &mat) {
        if (m_program == 0) return;
        use();

        // Get the location of the "modelMat" uniform variable
        GLint modelLoc = glGetUniformLocation(m_program, "modelMat");
        // Set the value of "modelMat"
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &mat[0][0]);
    }

    void Program::setViewMatrix(const glm::mat4 &mat) {
        if (m_program == 0) return;
        use();

        GLint viewLoc = glGetUniformLocation(m_program, "viewMat");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &mat[0][0]);
    }

    void Program::setProjectionMatrix(const glm::mat4 &mat) {
        if (m_program == 0) return;
        use();

        GLint projectionLoc = glGetUniformLocation(m_program, "projectionMat");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &mat[0][0]);
    }

    static GLuint compileShader(const char *src, GLenum type) {
        // Create the shader object
        GLuint shader = glCreateShader(type);
        // Load the shader source into the object
        glShaderSource(shader, 1, &src, nullptr);
        // Compile the shader
        glCompileShader(shader);

        // Check to see if the compilation was successful
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

        if (isCompiled == GL_FALSE) {
            // Get the length of the error message
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            //The maxLength includes the NULL character
            std::string infoLog(maxLength, 0);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            // Print the error mesage
            std::cerr << "Failed to compile shader:\n" << infoLog << std::endl;

            //We don't need the shader anymore.
            glDeleteShader(shader);

            // Abort the process
            abort();
        }

        return shader;
    }

	void Program::setBeta(const float &beta) {
		if (m_program == 0) return;
		use();

		// Get the location of the beta uniform variable
		GLint betaLoc = glGetUniformLocation(m_program, "beta");
		glUniform1f(betaLoc, beta);
	}

	void Program::setViewerPosition(const glm::vec3 &viewerPos) {
		if (m_program == 0) return;
		use();

		// Get the location of the viewer position uniform variable
		GLint viewerLoc = glGetUniformLocation(m_program, "viewerPos");
		glUniform3f(viewerLoc, viewerPos.x, viewerPos.y, viewerPos.z);
	}

	void Program::specifyLeafTexture() {
		glGenTextures(1, &leafTexture);
		glBindTexture(GL_TEXTURE_2D, leafTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height, nrChannels;
		unsigned char *data = stbi_load(CGRA_SRCDIR "/res/Textures/leaves.png", &width, &height, &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}


	void Program::setUpLeafBillboard(glm::mat4 ViewProjectionMatrix, glm::mat4 viewMatrix) {
		GLuint CameraRight_worldspace_ID = glGetUniformLocation(m_program, "CameraRight_worldspace");
		GLuint CameraUp_worldspace_ID = glGetUniformLocation(m_program, "CameraUp_worldspace");
		GLuint ViewProjMatrixID = glGetUniformLocation(m_program, "VP");
		GLuint BillboardPosID = glGetUniformLocation(m_program, "BillboardPos");
		GLuint BillboardSizeID = glGetUniformLocation(m_program, "BillboardSize");

		GLuint TextureID = glGetUniformLocation(m_program, "myTextureSampler");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, leafTexture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		glUniform3f(CameraRight_worldspace_ID, viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID, viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

		glUniform2f(BillboardSizeID, 0.1f, 0.1f);     // and 1m*12cm, because it matches its 256*32 resolution =)

		static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
		};
		GLuint billboard_vertex_buffer;
		glGenBuffers(1, &billboard_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			2,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
	}

	void Program::buildAirlightData(const char *filename, std::vector<GLubyte> &textureData, int uResolution, int vResolution) {
		textureData.resize(uResolution * vResolution);

		std::vector<GLubyte>::iterator currIt = textureData.begin();

		// Open the file
		std::ifstream file(filename, std::ios::in);
		if (!file.is_open()) {
			throw std::runtime_error("Error: could not open file for reading");
		}


		std::string row;
		
		std::getline(file, row); // X res
		std::getline(file, row); // Y res
		std::getline(file, row); // x min
		std::getline(file, row); // x max
		std::getline(file, row); // y min
		std::getline(file, row); // y max


		size_t lineNumber = 0;
		float integralResult;
		std::string stringResult;
		while (file.good()) {
			// Get the next line in the file
			std::getline(file, row);

			// Create a stream for the row
			std::istringstream lineStream(row);
			while (std::getline(lineStream, stringResult, ',')) {

				// Read next result
				integralResult = std::stof(stringResult);

				*currIt++ = (GLubyte)(integralResult * 255.0f);
			}
		}
	}
}
