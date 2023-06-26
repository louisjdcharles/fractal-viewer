#include "Viewer.h"

inline Viewer* getViewer(GLFWwindow* win) {
    return static_cast<Viewer*>(glfwGetWindowUserPointer(win));
}

void ScrollCallback(GLFWwindow* win, double x, double y) {
    Viewer* viewer = getViewer(win);

    viewer->CameraZoomFactor += y / 10.0;
}

void Viewer::Init()
{
    // vertex buffer for drawing output to screen
    glGenBuffers(1, &QuadVtxBufID);
    glBindBuffer(GL_ARRAY_BUFFER, QuadVtxBufID);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), &QUAD_VERTICES[0][0], GL_STATIC_DRAW);

    // index buffer for drawing output
    glGenBuffers(1, &QuadIdxBufID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIdxBufID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &QUAD_INDICES[0], GL_STATIC_DRAW);

    // vertex array for quad
    (glGenVertexArrays(1, &QuadVtxArrID));
    glBindVertexArray(QuadVtxArrID);
    
    GLuint offset = 0;

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    offset += 2 * sizeof(GL_FLOAT);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // output texture for compute shader
    glGenTextures(1, &ShaderOutTexID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ShaderOutTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, out_tex_w, out_tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, ShaderOutTexID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // create framebuffer, renderbuffer, and a texture to render to
    glGenFramebuffers(1, &FBO);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &RenderTexID);
    glBindTexture(GL_TEXTURE_2D, RenderTexID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, out_tex_w, out_tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexID, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, out_tex_w, out_tex_h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // shader to draw compute output
    GLuint vs = CompileShader("res/shaders/fractal_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fs = CompileShader("res/shaders/fragment.glsl", GL_FRAGMENT_SHADER);

    QuadShaderID = glCreateProgram();

    glAttachShader(QuadShaderID, fs);
    glAttachShader(QuadShaderID, vs);
    glLinkProgram(QuadShaderID);
    glValidateProgram(QuadShaderID);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glfwSetWindowUserPointer(NativeWindowPtr, (void*)this);

    glfwSetScrollCallback(NativeWindowPtr, ScrollCallback);
}

void Viewer::HandleMouseInput(const glm::vec3& world_pos, const glm::vec3& delta_world, bool button_down[5]) {
    if (ImGui::GetIO().MouseDown[0]) {
        if (MouseCapture == CaptureState::Inactive) {
            if (CurrentFractal.second->ShouldCaptureMouse(world_pos, CameraZoomFactor))
                MouseCapture = CaptureState::Captured;
            else
                MouseCapture = CaptureState::Pan;
        }

        if (MouseCapture == CaptureState::Captured)
        {
            CurrentFractal.second->HandleMouse(world_pos, delta_world);
        }
        else if (MouseCapture == CaptureState::Pan) {
            CameraPosition -= delta_world;
        }
    }
    else {
        MouseCapture = CaptureState::Inactive;
    }
}

void Viewer::RenderSettings()
{
    ImGui::Begin("Settings", nullptr);

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
    if (ImGui::BeginCombo("Current Fractal", CurrentFractal.first.c_str())) {

        for (auto const& kv : Fractals) {
            bool selected = (kv.first == CurrentFractal.first);

            if (ImGui::Selectable(kv.first.c_str(), selected)) {
                CurrentFractal = kv;
            }

            if (selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::Text("FPS: %d, Last Frame: %.2fms", (int)ImGui::GetIO().Framerate, ImGui::GetIO().DeltaTime * 1000.0);
    ImGui::Text("(%.4f, %.4f)", ImageToWorld(MousePos).x, ImageToWorld(MousePos).y);

    if (ImGui::Button("Reset Viewer")) {
        CameraPosition = {0, 0, 0};
        CameraZoomFactor = -1;
    }

    CurrentFractal.second->RenderImGui();

    ImGui::End();
}

glm::vec3 Viewer::ImageToWorld(ImVec2 pos) {

    float ratio = WindowSize.x / WindowSize.y;

    glm::vec3 coords = { ratio * (2.0 * pos.x / WindowSize.x - 1.0) , 1.0 - 2.0 * pos.y / WindowSize.y, 0 };
    coords = powf(2, -CameraZoomFactor) * coords;
    coords += CameraPosition;

    return coords;
}

void Viewer::Draw()
{
    ImGui::Begin("Viewer");
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    WindowSize = ImGui::GetContentRegionAvail();

    float ratio = WindowSize.x / WindowSize.y;

    glm::mat4 proj = glm::ortho(-ratio, ratio, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(glm::identity<glm::mat4>());
    glm::mat4 model = glm::scale(glm::identity<glm::mat4>(), glm::vec3(ratio, 1, 1));

    glm::mat4 pv_matrix = proj * view;
    MVP = pv_matrix * model;

    // calculate the position of the mouse, in pixels on the texture, and in world coordinates
    ImVec2 image_pos = ImGui::GetCursorScreenPos();
    ImVec2 mouse_screen_pos = ImGui::GetMousePos();

    LastMousePos = MousePos;
    MousePos = { mouse_screen_pos.x - image_pos.x, mouse_screen_pos.y - image_pos.y };
    glm::vec3 world_pos = ImageToWorld(MousePos);
    glm::vec3 last_world_pos = ImageToWorld(LastMousePos);

    // mouse position delta in world coordinates
    glm::vec3 delta = world_pos - last_world_pos;

    // check if view window is active, and the mouse is contained within the window
    if (ImGui::IsWindowFocused() &&
        MousePos.x < WindowSize.x &&
        MousePos.x > 0 &&
        MousePos.y < WindowSize.y &&
        MousePos.y > 0) {

        HandleMouseInput(world_pos, delta, ImGui::GetIO().MouseDown);
    }


    ImGui::Image(
        (ImTextureID) RenderTexID,
        WindowSize,
        { 0, WindowSize.y / out_tex_h },
        { WindowSize.x / out_tex_w, 0 }
    );

    ImGui::End();

    RenderSettings();

    CurrentFractal.second->Render(ShaderOutTexID, { WindowSize.x, WindowSize.y }, { out_tex_w, out_tex_h }, CameraZoomFactor, CameraPosition);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, WindowSize.x, WindowSize.y);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(QuadVtxArrID);
    glBindBuffer(GL_ARRAY_BUFFER, QuadVtxBufID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIdxBufID);
    glUseProgram(QuadShaderID);
    glActiveTexture(GL_TEXTURE0);;
    glBindTexture(GL_TEXTURE_2D, ShaderOutTexID);

    GLuint loc;
    glUseProgram(QuadShaderID);
    loc = glGetUniformLocation(QuadShaderID, "u_Texture");
    glUniform1i(loc, 0);
    loc = glGetUniformLocation(QuadShaderID, "u_Mvp");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);
    loc = glGetUniformLocation(QuadShaderID, "u_XScale");
    glUniform1f(loc, WindowSize.x / out_tex_w);
    loc = glGetUniformLocation(QuadShaderID, "u_YScale");
    glUniform1f(loc, WindowSize.y / out_tex_h);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    CurrentFractal.second->RenderInteractiveElements(pv_matrix, FBO, { WindowSize.x, WindowSize.y }, CameraZoomFactor, CameraPosition);
}

Viewer::Viewer(GLFWwindow* glfw_window, const FractalMap& fractals) : NativeWindowPtr(glfw_window), Fractals(fractals)
{
    CurrentFractal = *Fractals.begin();
}

Viewer::~Viewer()
{
    glDeleteRenderbuffers(1, &RBO);
    glDeleteFramebuffers(1, &FBO);

    glDeleteTextures(1, &RenderTexID);

    glDeleteVertexArrays(1, &QuadVtxArrID);
    glDeleteBuffers(1, &QuadVtxBufID);
    glDeleteBuffers(1, &QuadIdxBufID);

    glDeleteProgram(QuadShaderID);
    glDeleteTextures(1, &ShaderOutTexID);
}
