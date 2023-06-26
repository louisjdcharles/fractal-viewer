#include "Mandelbrot.h"

#include "Shader.h"

void Mandelbrot::Render(GLuint tex_out, glm::ivec2 out_dims, glm::ivec2 tex_dims, float zoom_factor, glm::vec3 focus) const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_out);

	GLuint loc;
	glUseProgram(ShaderID);
	loc = glGetUniformLocation(ShaderID, "u_MaxIter");
	glUniform1ui(loc, MaxIterations);
	loc = glGetUniformLocation(ShaderID, "u_Scale");
	glUniform1f(loc, pow(2, -zoom_factor));
	loc = glGetUniformLocation(ShaderID, "u_Focus");
	glUniform2f(loc, focus.x, focus.y);

	loc = glGetUniformLocation(ShaderID, "u_OutWidth");
	glUniform1i(loc, out_dims.x);
	loc = glGetUniformLocation(ShaderID, "u_OutHeight");
	glUniform1i(loc, out_dims.y);


	glDispatchCompute((GLuint)tex_dims.x / 32 + 1, (GLuint)tex_dims.y / 32 + 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Mandelbrot::InitShader()
{
	ShaderID = CreateComputeProgram("res/shaders/mandelbrot.glsl");
}
