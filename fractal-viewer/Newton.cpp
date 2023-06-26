#include "Newton.h"
#include <imgui/imgui.h>
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>

static const float QUAD_VERTICES[4][4]
// x y u v
{
    {-1, -1, 0.0f, 0.0f},
    {1, -1, 1.0, 0.0f},
    {1, 1, 1.0f, 1.0f},
    {-1, 1, 0.0f, 1.0f},
};

static const unsigned int QUAD_INDICES[] = {
    0, 1, 2,
    0, 2, 3
};

void Newton::InitShader()
{
    ShaderID = CreateComputeProgram("res/shaders/newtons_fractal.glsl");

    // initialise stuff for drawing circles

    glGenBuffers(1, &QuadVtxBufID);
    glBindBuffer(GL_ARRAY_BUFFER, QuadVtxBufID);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), &QUAD_VERTICES[0][0], GL_STATIC_DRAW);

    glGenBuffers(1, &QuadIdxBufID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIdxBufID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &QUAD_INDICES[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &QuadVtxArrID);
    glBindVertexArray(QuadVtxArrID);

    GLuint offset = 0;

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    offset += 2 * sizeof(GL_FLOAT);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint vs = CompileShader("res/shaders/vertex.glsl", GL_VERTEX_SHADER);
    GLuint fs = CompileShader("res/shaders/circle_fragment.glsl", GL_FRAGMENT_SHADER);

    CircleShaderID = glCreateProgram();

    glAttachShader(CircleShaderID, fs);
    glAttachShader(CircleShaderID, vs);
    glLinkProgram(CircleShaderID);
    glValidateProgram(CircleShaderID);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Newton::Render(GLuint tex_out, glm::ivec2 out_dims, glm::ivec2 tex_dims, float zoom_factor, glm::vec3 focus) const
{

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_out);

    GLuint loc;
    glUseProgram(ShaderID);
    loc = glGetUniformLocation(ShaderID, "u_MaxIter");
    glUniform1ui(loc, MaxIterations);
    loc = glGetUniformLocation(ShaderID, "u_ConvThreshold");
    glUniform1f(loc, ConvergenceThreshold);
    loc = glGetUniformLocation(ShaderID, "u_Scale");
    glUniform1f(loc, pow(2, -zoom_factor));
    loc = glGetUniformLocation(ShaderID, "u_Focus");
    glUniform2f(loc, focus.x, focus.y);

    loc = glGetUniformLocation(ShaderID, "u_Sol1");
    glUniform2f(loc, Sol1.x, Sol1.y);
    loc = glGetUniformLocation(ShaderID, "u_Sol2");
    glUniform2f(loc, Sol2.x, Sol2.y);
    loc = glGetUniformLocation(ShaderID, "u_Sol3");
    glUniform2f(loc, Sol3.x, Sol3.y);

    loc = glGetUniformLocation(ShaderID, "u_OutWidth");
    glUniform1i(loc, out_dims.x);
    loc = glGetUniformLocation(ShaderID, "u_OutHeight");
    glUniform1i(loc, out_dims.y);


    glDispatchCompute((GLuint) tex_dims.x / 32 + 1, (GLuint) tex_dims.y / 32 + 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Newton::RenderInteractiveElements(const glm::mat4& pv_mat, GLuint target_fbo, glm::ivec2 window_dims, float zoom_factor, glm::vec3 focus)
{
    glBindFramebuffer(GL_FRAMEBUFFER, target_fbo);

    glBindVertexArray(QuadVtxArrID);
    glBindBuffer(GL_ARRAY_BUFFER, QuadVtxBufID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIdxBufID);
    glUseProgram(CircleShaderID);

    float scale_factor = pow(2, zoom_factor);

    auto DrawCircle = [&](glm::vec3 position, float radius, const glm::vec4&& colour) {
        glm::mat4 circleMM = glm::identity<glm::mat4>();
        circleMM = glm::translate(circleMM, scale_factor * (position - focus));
        circleMM = glm::scale(circleMM, glm::vec3(radius, radius, 1.0f));

        glm::mat4 mvp = pv_mat * circleMM;

        GLuint loc;
        loc = glGetUniformLocation(CircleShaderID, "u_Mvp");
        glUniformMatrix4fv(loc, 1, GL_FALSE, &mvp[0][0]);
        loc = glGetUniformLocation(CircleShaderID, "u_CircleColour");
        glUniform4f(loc, colour.r, colour.g, colour.b, colour.a);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    };

    DrawCircle(Sol1, 0.02, { 0.961, 0.498, 0.733, 1 });
    DrawCircle(Sol2, 0.02, { 0.961, 0.918, 0.592, 1 });
    DrawCircle(Sol3, 0.02, { 0.690, 0.929, 0.961, 1 });
}

void Newton::RenderImGui()
{
    Fractal::RenderImGui();
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
    ImGui::DragFloat("Convergence Threshold", &ConvergenceThreshold, 0.001, 0, 1);
}

bool Newton::ShouldCaptureMouse(const glm::vec3& m_world_pos, float zoom_factor)
{
    float scale_factor = powf(2, -zoom_factor);

    if (glm::distance(Sol1, m_world_pos) < 0.02 * scale_factor) {
        CurrentSelectedSol = SelectedSol::Sol1;
        return true;
    }

    if (glm::distance(Sol2, m_world_pos) < 0.02 * scale_factor) {
        CurrentSelectedSol = SelectedSol::Sol2;
        return true;
    }

    if (glm::distance(Sol3, m_world_pos) < 0.02 * scale_factor) {
        CurrentSelectedSol = SelectedSol::Sol3;
        return true;
    }

    CurrentSelectedSol = SelectedSol::None;
    return false;
}

void Newton::HandleMouse(const glm::vec3& m_world_pos, const glm::vec3& m_world_delta)
{
    switch (CurrentSelectedSol) {
    case SelectedSol::Sol1:
        Sol1 = m_world_pos;
        break;
    case SelectedSol::Sol2:
        Sol2 = m_world_pos;
        break;
    case SelectedSol::Sol3:
        Sol3 = m_world_pos;
        break;
    }
}

Newton::~Newton()
{
    glDeleteBuffers(1, &QuadIdxBufID);
    glDeleteBuffers(1, &QuadVtxBufID);
    glDeleteVertexArrays(1, &QuadVtxArrID);

    glDeleteProgram(CircleShaderID);
}
