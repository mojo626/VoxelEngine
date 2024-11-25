#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <map>

#include "common.h"
#include "window.hpp"

struct CamData {
    float camData[2][4];
};


class Player {
    public:

        Player() {
            
            camData[0][0] = 0.0;
            camData[0][1] = 0.0;
            camData[0][2] = 0.0;

            camData[1][0] = 0.0;
            camData[1][1] = 0.0;
            camData[1][2] = 1.0;
        }

        void update(std::map<int, bool> keyboard) {

            if (keyboard[GLFW_KEY_A])
            {
                float rightX = cos(camYaw + (3.1415 / 2));
                float rightZ = sin(camYaw + (3.1415 / 2));

                camData[0][2] += rightZ * walkSpeed;
                camData[0][0] += rightX * walkSpeed;
            }
            if (keyboard[GLFW_KEY_D])
            {
                float rightX = cos(camYaw + (3.1415 / 2));
                float rightZ = sin(camYaw + (3.1415 / 2));

                camData[0][2] -= rightZ * walkSpeed;
                camData[0][0] -= rightX * walkSpeed;
            }
            if (keyboard[GLFW_KEY_W])
            {
                float forwardX = cos(camYaw);
                float forwardZ = sin(camYaw);

                camData[0][2] += forwardZ * walkSpeed;
                camData[0][0] += forwardX * walkSpeed;
            }
            if (keyboard[GLFW_KEY_S])
            {
                float forwardX = cos(camYaw);
                float forwardZ = sin(camYaw);

                camData[0][2] -= forwardZ * walkSpeed;
                camData[0][0] -= forwardX * walkSpeed;
            }

            if (keyboard[GLFW_KEY_SPACE])
            {
                camData[0][1] += walkSpeed;
            }

            if (keyboard[GLFW_KEY_LEFT_SHIFT])
            {
                camData[0][1] -= walkSpeed;
            }


            camData[1][0] = cos(camYaw) * cos(camPitch);
            camData[1][2] = sin(camYaw) * cos(camPitch);
            camData[1][1] = sin(camPitch);
        }

        CamData getCamData() {
            CamData data;
            data.camData[0][0] = camData[0][0];
            data.camData[0][1] = camData[0][1];
            data.camData[0][2] = camData[0][2];

            data.camData[1][0] = camData[1][0];
            data.camData[1][1] = camData[1][1];
            data.camData[1][2] = camData[1][2];
            return data;
        }

        void handleMouseMove(float dx, float dy) {
            camYaw -= dx * mouseSensitivity;
            camPitch += dy * mouseSensitivity;

            if (camPitch >= 3.1415/2)
            {
                camPitch = 3.1415/2;
            }
            if (camPitch <= -3.1415/2)
            {
                camPitch = -3.1415/2;
            }
        }

    private:
        float camData[2][4];
        float walkSpeed = 0.1;
        float camYaw = 0.0f;
        float camPitch = 0.0f;
        float mouseSensitivity = 0.005f;
       
};



#endif