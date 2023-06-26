#pragma once
#include "Fractal.h"
class Mandelbrot : public Fractal
{
public:
	Mandelbrot() {};

	void Render(GLuint tex_out, glm::ivec2 out_dims, glm::ivec2 tex_dims, float zoom_factor, glm::vec3 focus) const;
	void InitShader();
};

