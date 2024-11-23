#include <iostream>


#include <map>

// #include "window.hpp"
#include "color.hpp"

#include "common.h"
#include "window.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600





int main() {
    
    Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT);

    if (!window.init())
        return 1;


    while (!glfwWindowShouldClose(window.getWindow())) {
		
        window.startRendering();

		// Use debug font to print information about this example.
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
		// Enable stats or debug text.
		bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);

        window.finishRendering();
		
	}
	
    window.destroy();
    return EXIT_SUCCESS;
}