//===============================================================================
// desc: A manager class with a collection of handy utilies for GLFW windows, useful for both DX and Vulkan
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "WindowGLFW.h"
#include <AetherUtils.h>
//===============================================================================

namespace Aether
{
    static bool s_bInitGLFW = false;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    WindowGLFW::WindowGLFW(const WinData& winData)
    {
        AETHER_ASSERT(Initialize(winData));
        // Don't forget to to set this winData for use later!
        SetData(winData);
    }

    WindowGLFW::~WindowGLFW()
    {
        Terminate();
    }

    AETHER_RESULT WindowGLFW::Initialize(const WinData& winData)
    {
        // Get GLFW setup for app windows - only do this once!
        if (!s_bInitGLFW)
        {
            if (!glfwInit())
                assert(false && "Failed to initialize GLFW.");

            s_bInitGLFW = true;
        }
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

    bool WindowGLFW::WindowShouldClose()
    {
        // Return a flag that is set when the user attempts to close the window, but the window isn't actually closed yet.
        return glfwWindowShouldClose(m_pWindow);
    }

    void WindowGLFW::SetEventCallback(const EventCallbackFn& callback)
    {

    }

    void WindowGLFW::Terminate()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }
};