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
        AETHER_ASSERT(Initialize(winData), "Failed to initalize GLFW properly.");
        // Don't forget to to set this winData for use later!
        SetData(winData);
//        m_GLFWData.m_BaseWinData = winData;
    }

    WindowGLFW::~WindowGLFW()
    {
        Terminate();
    }

    AETHER_RESULT WindowGLFW::Initialize(const WinData& winData)
    {
        AETHER_RESULT ar = AETHER_OK;

        // Get GLFW setup for all our windows - only do this once!
        if (!s_bInitGLFW)
        {
            ar = glfwInit() ? AETHER_OK : AETHER_FAIL;    // GLFW returns true or false, this doesn't map cleanly with our result so use a ternary to finagle it!
            AETHER_ASSERT(ar, "Failed to initialize GLFW with glfwInit(). Do you have the library built?");
            s_bInitGLFW = true;
        }
        // Create a GLFW window without an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
       
        // TODO: Dynamic window size

        m_pWindow = glfwCreateWindow(winData.m_ClientWidth, winData.m_ClientHeight, winData.m_Title.c_str(), nullptr, nullptr);
        if (m_pWindow == nullptr)
        {
            glfwTerminate();
            AETHER_ASSERT(AETHER_FAIL, "Failed to create GLFW window. Check window creation data.");
        }

        // Set callback function to handle inputs
        glfwMakeContextCurrent(m_pWindow);
        glfwSetWindowUserPointer(m_pWindow, &m_WinData);
        glfwSetKeyCallback(m_pWindow, key_callback);


        return ar;
    }

    bool WindowGLFW::WindowShouldClose()
    {
        // Return a flag that is set when the user attempts to close the window, but the window isn't actually closed yet.
        return glfwWindowShouldClose(m_pWindow);
    }

    void WindowGLFW::Terminate()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }
};