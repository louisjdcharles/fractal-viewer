#include <glad/glad.h>

#include <iostream>
#include <map>

#include "Newton.h"
#include "Mandelbrot.h"
#include "Viewer.h"

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// callback to make window fullscreen
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (glfwGetWindowMonitor(window) != NULL)
		{
			glfwSetWindowMonitor(window, NULL, 200, 200, 1280, 720, GLFW_DONT_CARE);
		}
		else
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
		}
	}
}

int main(void)
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Fractal Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	{
		// load opengl functions
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize OpenGL context" << std::endl;
			return -1;
		}

		std::cout << glGetString(GL_VERSION) << "\n";

		// imgui initialisation
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", 16);
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		auto mandelbrot_ptr = std::make_shared<Mandelbrot>();
		mandelbrot_ptr->InitShader();
		auto newton_ptr = std::make_shared<Newton>();
		newton_ptr->InitShader();

		FractalMap fractals;
		fractals.emplace("Mandelbrot", mandelbrot_ptr);
		fractals.emplace("Newton's Fractal", newton_ptr);

		Viewer viewer(window, fractals);

		viewer.Init();

		while (!glfwWindowShouldClose(window))
		{
			int width, height;

			glfwGetFramebufferSize(window, &width, &height);
			// glfwSwapInterval(1)

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// imgui ui
			{

				ImGui::SetNextWindowPos({ 0, 0 });
				ImGui::SetNextWindowSize({ (float)width, (float)height });
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
				ImGui::Begin("Fractals", nullptr, ImGuiWindowFlags_NoTitleBar);
				ImGui::PopStyleVar();
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

				// draw viewer windows
				viewer.Draw();

				ImGui::End();
			}

			ImGui::Render();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, width, height);

			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);

			glfwPollEvents();
		}

	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}