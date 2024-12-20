#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <map>

#include "common.h"
#include "player.hpp"


static bool s_showStats = false;



class Window {
    public:
        std::map<int, bool> keyboard;
        GLFWwindow* window;
        Player* player;
        float lastX = 400, lastY = 300;

        Window(int windowWidth2, int windowHeight2, Player* player2)
        {
            windowWidth = windowWidth2;
            windowHeight = windowHeight2;
            player = player2;

            lastX = windowWidth / 2;
            lastY = windowHeight / 2;

        }

        bool init() {
            glfwSetErrorCallback(glfw_errorCallback);
            if (!glfwInit())
                return false;
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            window = glfwCreateWindow(windowWidth, windowHeight, "helloworld", nullptr, nullptr);
            if (!window)
                return false;

            glfwSetWindowUserPointer(window, this);

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPosCallback(window, mouse_callback);

            glfwSetKeyCallback(window, glfw_keyCallback);


            // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
            // Most graphics APIs must be used on the same thread that created the window.
            bgfx::renderFrame();
            // Initialize bgfx using the native window handle and window resolution.
            bgfx::Init init;
            #if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
                init.platformData.ndt = glfwGetX11Display();
                init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
            #elif BX_PLATFORM_OSX
                init.platformData.nwh = glfwGetCocoaWindow(window);
                init.type = bgfx::RendererType::Metal;
            #elif BX_PLATFORM_WINDOWS
                init.platformData.nwh = glfwGetWin32Window(window);
                init.type = bgfx::RendererType::Direct3D11;
            #endif

            int width, height;
            glfwGetWindowSize(window, &width, &height);
            init.resolution.width = (uint32_t)width;
            init.resolution.height = (uint32_t)height;
            init.resolution.reset = BGFX_RESET_VSYNC;
            if (!bgfx::init(init))
                return false;

            const bgfx::Caps* caps = bgfx::getCaps();
            bool m_computeSupported  = !!(caps->supported & BGFX_CAPS_COMPUTE);

            if (!m_computeSupported)
            {
                std::cout << "Compute shaders are not supported..." << std::endl;
                return false;
            }

            kClearView = 0;
            bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
            bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);


            return true;
        }

        void destroy()
        {
            bgfx::shutdown();
	        glfwTerminate();
        }

        GLFWwindow* getWindow()
        {
            return window;
        }


        void startRendering()
        {
            glfwPollEvents();
            // Handle window resize.
            int oldWidth = windowWidth, oldHeight = windowHeight;
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            if (width != oldWidth || height != oldHeight) {
                bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
                bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
            }


            // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
            bgfx::touch(kClearView);
            // Use debug font to print information about this example.
            bgfx::dbgTextClear();

            bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to view stats");
            bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
        }

   

        void finishRendering()
        {
            // Advance to next frame. Process submitted rendering primitives.
		    bgfx::frame();
        }
    
    private:
        int windowWidth;
        int windowHeight;
        bgfx::ViewId kClearView;
        

        static void glfw_errorCallback(int error, const char *description)
        {
            fprintf(stderr, "GLFW error %d: %s\n", error, description);
        }

        static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
        {
            Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
                s_showStats = !s_showStats;
            if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
                glfwSetWindowShouldClose(window, 1);

            if (action == GLFW_PRESS)
                win->keyboard[key] = true;
            else if (action == GLFW_RELEASE)
                win->keyboard[key] = false;
        }

        static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
            Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

            float xoffset = xpos - win->lastX;
            float yoffset =
                win->lastY - ypos; // reversed since y-coordinates range from bottom to top
            win->lastX = xpos;
            win->lastY = ypos;

            win->player->handleMouseMove(xoffset, yoffset);
        }
        

};


#endif