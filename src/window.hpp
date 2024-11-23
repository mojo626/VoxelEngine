#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

#include <iostream>

#include "color.hpp"

class Window {
    public:
        Window(int windowWidth2, int windowHeight2)
        {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_CreateWindowAndRenderer("window", windowWidth2, windowHeight2, 0, &window, &renderer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            windowWidth = windowWidth2;
            windowHeight = windowHeight2;
        }

        void destroy()
        {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        SDL_Renderer* getRenderer() { return renderer; }

        SDL_Window* getWindow() { return window; }

        SDL_Event* getEvent() { return &event; }

        void startRendering()
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            
            screenTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);
            void* pixels;
            int pitch;

            // SDL_RendererInfo rendererInfo;
            // SDL_GetRendererInfo(renderer, &rendererInfo);

            SDL_PixelFormat format = SDL_GetWindowPixelFormat(window);
            pixelFormat = SDL_GetPixelFormatDetails(format);
            

            SDL_LockTexture(screenTex, NULL, &pixels, &pitch);

            pixelArray = (Uint32*)pixels;
        }

        void drawPoint(int x, int y, Color color)
        {   
            pixelArray[x + y * windowWidth] = SDL_MapRGBA(pixelFormat, NULL, color.r, color.g, color.b, 255);
        }

        void drawVertLine(int start, int stop, int x, Color color)
        {
            for (int y = start; y <= stop; y++)
            {
                drawPoint(x, y, color);
            }
        }

        void finishRendering()
        {
            SDL_UnlockTexture(screenTex);
    
            SDL_RenderTexture(renderer, screenTex, NULL, NULL);

            SDL_DestroyTexture(screenTex);

            SDL_RenderPresent(renderer);

            pixelArray = NULL;
            pixelFormat = NULL;
        }
    
    private:
        SDL_Renderer* renderer;
        SDL_Window* window;
        SDL_Event event;
        Uint32* pixelArray;
        const SDL_PixelFormatDetails* pixelFormat;
        SDL_Texture* screenTex;
        int windowWidth;
        int windowHeight;

};


#endif