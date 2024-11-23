#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#include "common.h"

#include "color.hpp"

static bool s_showStats = false;

void glfw_errorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
        s_showStats = !s_showStats;
}

class Window {
    public:
        Window(int windowWidth2, int windowHeight2)
        {
            windowWidth = windowWidth2;
            windowHeight = windowHeight2;

        }

        bool init() {
            glfwSetErrorCallback(glfw_errorCallback);
            if (!glfwInit())
                return false;
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            window = glfwCreateWindow(windowWidth, windowHeight, "helloworld", nullptr, nullptr);
            if (!window)
                return false;

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
            #elif BX_PLATFORM_WINDOWS
                init.platformData.nwh = glfwGetWin32Window(window);
            #endif

            int width, height;
            glfwGetWindowSize(window, &width, &height);
            init.resolution.width = (uint32_t)width;
            init.resolution.height = (uint32_t)height;
            init.resolution.reset = BGFX_RESET_VSYNC;
            if (!bgfx::init(init))
                return false;

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
        }

   

        void finishRendering()
        {
            // Advance to next frame. Process submitted rendering primitives.
		    bgfx::frame();
        }
    
    private:
        int windowWidth;
        int windowHeight;
        GLFWwindow* window;
        bgfx::ViewId kClearView;

};


#endif