#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <map>

#include "window.hpp"
#include "color.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

std::map<int, bool> keyboard;

void update(Window* window)
{
    window->startRendering();

    for (int y = 0; y < WINDOW_HEIGHT; y++)
    {
        window->drawPoint(y, y, Color(255, 0, 0));
    }

    window->finishRendering();
    
}

int main() {
    
    Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT);

    double newTime = 0.0;
    bool done = false;
    while (!done) {
        double oldTime = newTime;
        newTime = SDL_GetTicks();

        SDL_Event event = *window.getEvent();

        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_EVENT_KEY_DOWN:
                    keyboard[event.key.raw] = true;
                    break;
                case SDL_EVENT_KEY_UP:
                    keyboard[event.key.raw] = false;
                    break;
                case SDL_EVENT_QUIT:
                    done = true;
            }
        }


        update(&window);

        double fps = 1.0 / ((newTime - oldTime)/1000.0);

        SDL_SetWindowTitle(window.getWindow(), ("Renderer | " + std::to_string(fps)).c_str());

    }


    window.destroy();
    return EXIT_SUCCESS;
}