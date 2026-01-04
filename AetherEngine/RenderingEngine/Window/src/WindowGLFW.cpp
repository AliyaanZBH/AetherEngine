//===============================================================================
// desc: A manager class with a collection of handy utilies for GLFW windows, useful for both DX and Vulkan
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "WindowGLFW.h"
#include "AetherUtils.h"
#include "AppEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "GraphicsContext.h"
//===============================================================================

namespace Aether
{
    static bool s_bInitGLFW = false;

    // This one is static for now until I create a proper dedicated error callback func
    static void GLFWErrorCallback(int error, const char* description)
    {
        AETHER_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    WindowGLFW::WindowGLFW(const WindowContext::WinData& winData, const eRenderAPI currentRenderer)
    {
        AETHER_ASSERT(Initialize(winData, currentRenderer), "Failed to initalize GLFW properly.");
        
        // Don't forget to to set this winData for use later!
        SetData(winData);
    }

    WindowGLFW::~WindowGLFW()
    {
        Terminate();
    }

    AETHER_RESULT WindowGLFW::Initialize(const WindowContext::WinData& winData, const eRenderAPI currentRenderer)
    {
        AETHER_RESULT ar = AETHER_OK;
        // Get GLFW setup for all our windows - only do this once!
        if (!s_bInitGLFW)
        {
            ar = glfwInit() ? AETHER_OK : AETHER_FAIL;    // GLFW returns true or false, this doesn't map cleanly with our result so use a ternary to finagle it!
            AETHER_ASSERT(ar, "Failed to initialize GLFW with glfwInit(). Do you have the library built?");

           // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            // Set error callback to catch those errors!
            glfwSetErrorCallback(GLFWErrorCallback);

            s_bInitGLFW = true;
        }

        switch (currentRenderer)
        {
            // Ensure GLFW window doesn't hijack the hwnd by creating an OpenGL context automatically if we're using DirectX
            case eRenderAPI::kDX11:
            case eRenderAPI::kDX12:
            {
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
            }
            case eRenderAPI::kOpenGL:
            {
                // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                break;
            }
            default:
                break;
        }
        
        // VSync
        //glfwSwapInterval(1);

        m_pWindow = glfwCreateWindow(winData.m_ClientWidth, winData.m_ClientHeight, winData.m_Title.c_str(), nullptr, nullptr);
        if (m_pWindow == nullptr)
        {
            glfwTerminate();
            AETHER_ASSERT(AETHER_FAIL, "Failed to create GLFW window. Check window creation data.");
        }

        // Set User pointer which contains the callback function to handle events!
        glfwSetWindowUserPointer(m_pWindow, &m_WinData);


        //
        //  Callback setup
        //

        // Window Resise
        //
        glfwSetWindowSizeCallback(m_pWindow, [] (GLFWwindow* window, int width, int height)
        {
            // Pull out user data blob, cast it to our custom struct and then dereference
            WindowContext::WinData& data = *(WindowContext::WinData*)glfwGetWindowUserPointer(window);

            // Update sizes then create event  and dispatch via the callback member
            data.m_ClientWidth = width;
            data.m_ClientHeight = height;

            // Register event
            WindowResizeEvent event(width, height);
            data.m_EventCallback(event);
        });

        //
        //  Repeat for all events!
        //

        glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window)
        {
            WindowContext::WinData& data = *(WindowContext::WinData*)glfwGetWindowUserPointer(window);

            WindowCloseEvent event;
            data.m_EventCallback(event);

            // Also, close window! 
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        });

        glfwSetWindowFocusCallback(m_pWindow, [] (GLFWwindow* window, int focused)
        {
            WindowContext::WinData& data = *(WindowContext::WinData*)glfwGetWindowUserPointer(window);

            WindowFocusEvent event(focused);
            data.m_EventCallback(event);
        });


        // Key Callback
        //
        glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowContext::WinData& data = *(WindowContext::WinData*)glfwGetWindowUserPointer(window);

            // Map GLFW input events to our own
            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, 0);
                    data.m_EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    data.m_EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, 1);
                    data.m_EventCallback(event);
                    break;
                }
            }

            // Also, close window if escape is the specific key pressed. Hardcoded for now
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GLFW_TRUE);
        });

        glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowContext::WinData& data = *(WindowContext::WinData*)glfwGetWindowUserPointer(window);

            // Map GLFW mouse events - these are actually the same as keyboard presses!
            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseClickEvent event(button);
                    data.m_EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseClickReleaseEvent event(button);
                    data.m_EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_pWindow, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            WindowContext::WinData& data = *(WindowContext::WinData*)glfwGetWindowUserPointer(window);

            MouseScrollEvent event((float)xOffset, (float)yOffset);
            data.m_EventCallback(event);
        });

        glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double xPos, double yPos)
        {
            WindowContext::WinData& data = *(WindowContext::WinData*)glfwGetWindowUserPointer(window);

            MouseMoveEvent event((float)xPos, (float)yPos);
            data.m_EventCallback(event);
        });


        return ar;
    }

    bool WindowGLFW::WindowShouldClose()
    {
        // Return a flag that is set when the user attempts to close the window, but the window isn't actually closed yet.
        return glfwWindowShouldClose(m_pWindow);
    }

    inline void WindowGLFW::SetCursorLocked(const bool lock)
    {
        glfwSetInputMode(m_pWindow, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    void WindowGLFW::Terminate()
    {
        glfwDestroyWindow(m_pWindow);
        glfwTerminate();
    }
};