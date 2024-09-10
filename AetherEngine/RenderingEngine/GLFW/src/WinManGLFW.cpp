#include "WinManGLFW.h"
#include <cassert>

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool WinManGLFW::Initialize(const WinData& winData)
{
    // Get GLFW setup for app window
    if (!glfwInit())
        assert(false && "Failed to initialize GLFW.");

    // Create a GLFW window without an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // TODO: Dynamic window size

    m_pWindow = glfwCreateWindow(winData.m_ClientWidth, winData.m_ClientHeight, "Aether Engine", nullptr, nullptr);
    if (m_pWindow == nullptr)
    {
        assert(false && "Failed to create GLFW window.");
        glfwTerminate();
        return false;
    }

    // Set callback function to handle inputs
    glfwSetKeyCallback(m_pWindow, key_callback);

    return true;
}

bool WinManGLFW::WindowShouldClose()
{
    // Return a flag that is set when the user attempts to close the window, but the window isn't actually closed yet.
    return glfwWindowShouldClose(m_pWindow);
}