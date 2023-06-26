#include "Fractal.h"

#include <imgui/imgui.h>

void Fractal::InitShader()
{
}

void Fractal::Render(GLuint tex_out, glm::ivec2 out_dims, glm::ivec2 tex_dims, float zoom_factor, glm::vec3 focus) const
{
}

void Fractal::RenderImGui()
{
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
	ImGui::DragInt("Maximum Iterations", &MaxIterations, 0.25 * log2(MaxIterations), 1.0f, 10000.0f);
}

void Fractal::RenderInteractiveElements(const glm::mat4& vp_mat, GLuint target_fbo, glm::ivec2 window_dims, float zoom_factor, glm::vec3 focus)
{
}

Fractal::~Fractal()
{
	glDeleteProgram(ShaderID);
}

bool Fractal::ShouldCaptureMouse(const glm::vec3& m_world_pos, float zoom_factor)
{
	return false;
}

void Fractal::HandleMouse(const glm::vec3& m_world_pos, const glm::vec3& m_world_delta)
{
}
