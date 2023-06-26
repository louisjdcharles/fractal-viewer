#pragma once

#include "Files.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <glad/glad.h>

// for vertex and fragment shaders
GLint CompileShader(const std::string& path, GLenum type);

GLuint CreateComputeProgram(const std::string& path);