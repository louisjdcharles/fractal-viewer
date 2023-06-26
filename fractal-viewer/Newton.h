#pragma once

#include <glm/glm.hpp>
#include "Files.h"

#include "Fractal.h"

enum class SelectedSol {
	Sol1,
	Sol2,
	Sol3,
	None
};

class Newton : public Fractal
{
private:
	GLuint QuadVtxBufID;
	GLuint QuadIdxBufID;
	GLuint QuadVtxArrID;
	GLuint CircleShaderID;
	SelectedSol CurrentSelectedSol = SelectedSol::None;

	float ConvergenceThreshold = 0.1;
public:
	glm::vec3 Sol1 = { 1, 0, 0 }, Sol2 = { -0.5, 0.86, 0 }, Sol3 = { -0.5, -0.86, 0 };

	Newton() {};

	void InitShader();

	void Render(GLuint tex_out, glm::ivec2 out_dims, glm::ivec2 tex_dims, float zoom_factor, glm::vec3 focus) const;
	void RenderInteractiveElements(const glm::mat4& vp_mat, GLuint target_fbo, glm::ivec2 window_dims, float zoom_factor, glm::vec3 focus);
	void RenderImGui();
	bool ShouldCaptureMouse(const glm::vec3& m_world_pos, float zoom_factor);
	void HandleMouse(const glm::vec3& m_world_pos, const glm::vec3& m_world_delta);

	~Newton();
};
