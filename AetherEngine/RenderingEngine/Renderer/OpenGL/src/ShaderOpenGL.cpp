#pragma once
//===============================================================================
// desc: Shader class implementation for OpenGL
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "ShaderOpenGL.h"
//===============================================================================

namespace Aether
{
	ShaderOpenGL::ShaderOpenGL(const std::string& vertSrc, const std::string& fragSrc)
	{
		//
		//	Taken more or less verbatim from Khronos doc: https://wikis.khronos.org/opengl/Shader_Compilation
		//

		// Read our shaders into the appropriate buffers

		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = (const GLchar*)vertSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			// Use the infoLog as you see fit.
			AETHER_ASSERT(AETHER_FAIL, "Vertex shader failed to compile: {0}", infoLog.data());

			// In this simple program, we'll just leave
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)fragSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(fragmentShader);
			// Delete both of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			AETHER_ASSERT(AETHER_FAIL, "Fragment shader failed to compile: {0}", infoLog.data());
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_GLProgram = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_GLProgram, vertexShader);
		glAttachShader(m_GLProgram, fragmentShader);

		// Link our program
		glLinkProgram(m_GLProgram);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_GLProgram, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_GLProgram, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_GLProgram, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_GLProgram);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			AETHER_ASSERT(AETHER_FAIL, "Shaders failed to link: {0}", infoLog.data());
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_GLProgram, vertexShader);
		glDetachShader(m_GLProgram, fragmentShader);
	}

	void ShaderOpenGL::Bind()
	{
		glUseProgram(m_GLProgram);
	}
	void ShaderOpenGL::Unbind()
	{
		glUseProgram(0);
	}
}