#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Fractal
{
protected:
	GLuint ShaderID;
	int MaxIterations = 200;
public:
	virtual void InitShader();
	virtual void Render(GLuint tex_out, glm::ivec2 out_dims, glm::ivec2 tex_dims, float zoom_factor, glm::vec3 focus) const;
	virtual void RenderImGui();
	virtual void RenderInteractiveElements(const glm::mat4& vp_mat, GLuint target_fbo, glm::ivec2 window_dims, float zoom_factor, glm::vec3 focus);
	~Fractal();
	virtual bool ShouldCaptureMouse(const glm::vec3& m_world_pos, float zoom_factor);
	virtual void HandleMouse(const glm::vec3& m_world_pos, const glm::vec3& m_world_delta);
};

