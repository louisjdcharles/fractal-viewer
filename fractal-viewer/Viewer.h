#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "imgui/imgui.h"
#include "Shader.h"
#include "Fractal.h"

#include <memory>
#include <map>

#include <GLFW/glfw3.h>

using FractalKV = std::pair<std::string, std::shared_ptr<Fractal>>;
using FractalMap = std::unordered_map<std::string, std::shared_ptr<Fractal>>;

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

enum class CaptureState {
    Inactive,
    Captured, // Mouse movement used by fractal, fractal always has higher priority than the viewer when processing
    Pan
};

class Viewer
{
private:
    GLuint QuadVtxBufID;
    GLuint QuadIdxBufID;

    GLuint FBO;
    GLuint RBO;
    GLuint RenderTexID;

    GLuint ShaderOutTexID;
    GLuint QuadShaderID;
    GLuint QuadVtxArrID;

    glm::vec3 CameraPosition = {0, 0, 0};
	float CameraZoomFactor = -1;

    glm::mat4 MVP;

    ImVec2 LastMousePos = {0, 0};
    ImVec2 MousePos = { 0, 0 };

    CaptureState MouseCapture = CaptureState::Inactive;

    ImVec2 WindowSize;

    FractalKV CurrentFractal;
    FractalMap Fractals;

    GLFWwindow* NativeWindowPtr;

    const int out_tex_w = 4096, out_tex_h = 4096;

    glm::vec3 ImageToWorld(ImVec2 pos);

    void HandleMouseInput(const glm::vec3& world_pos, const glm::vec3& delta_world, bool button_down[5]);

    void RenderSettings();
public:
    Viewer(GLFWwindow* glfw_window, const FractalMap& fractals);
    ~Viewer();

    void Init();
    void Draw();

    static friend void ScrollCallback(GLFWwindow* win, double x, double y);
};

