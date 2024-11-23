#include <iostream>


#include <map>

#include "common.h"
#include "window.hpp"


#include "../res/shaders/basic/f_shader.fs.h"
#include "../res/shaders/basic/v_shader.vs.h"
#include "../res/shaders/basic/gol.cs.h"
#include "../res/shaders/basic/voxel.cs.h"

#include "ComputeManager.hpp"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct Pos {
    float x;
    float y;
    float z;
};

static const Pos s_quadVertices[] =
{
	{ 1.0f,  1.0f, 0.0f},
	{-1.0f,  1.0f, 0.0f},
	{-1.0f, -1.0f, 0.0f},
	{ 1.0f, -1.0f, 0.0f},
};

static const uint16_t s_quadIndices[] = { 0, 1, 2, 2, 3, 0, };




int main() {

    ComputeManager compute(128, 128);
    
    Window window = Window(WINDOW_WIDTH, WINDOW_HEIGHT);

    if (!window.init())
        return 1;


    //Setting up main shader
    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .end();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_quadVertices, sizeof(s_quadVertices)), pcvDecl);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(s_quadIndices, sizeof(s_quadIndices)));

    bgfx::UniformHandle tex = bgfx::createUniform("tex", bgfx::UniformType::Sampler);
    bgfx::UniformHandle texSize = bgfx::createUniform("texSize", bgfx::UniformType::Vec4);

    bgfx::ShaderHandle vsh = bgfx::createShader( bgfx::makeRef(v_shader_vs, sizeof(v_shader_vs)) );
    bgfx::ShaderHandle fsh = bgfx::createShader( bgfx::makeRef(f_shader_fs, sizeof(f_shader_fs)) );
    
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    bgfx::UniformHandle camUniform = bgfx::createUniform("camData", bgfx::UniformType::Vec4, 2);
    bgfx::UniformHandle sizeUniform = bgfx::createUniform("outputSize", bgfx::UniformType::Vec4, 1);

    compute.init();

    float camData[2][4];
    camData[0][0] = 0.0;
    camData[0][1] = 0.0;
    camData[0][2] = 0.0;

    camData[1][0] = 0.0;
    camData[1][1] = 0.0;
    camData[1][2] = 1.0;

    float sizeData[4];
    sizeData[0] = 128;
    sizeData[1] = 128;

    float camYaw = 0.0f;
    float camPitch = 0.0f;
    float walkSpeed = 0.1f;

    bgfx::setUniform(sizeUniform, sizeData, 1);

    while (!glfwWindowShouldClose(window.getWindow())) {


        window.startRendering();

        if (window.keyboard[GLFW_KEY_A])
        {
            float rightX = cos(camYaw + (3.1415 / 2));
            float rightZ = sin(camYaw + (3.1415 / 2));

            camData[0][2] += rightZ * walkSpeed;
            camData[0][0] += rightX * walkSpeed;
        }
        if (window.keyboard[GLFW_KEY_D])
        {
            float rightX = cos(camYaw + (3.1415 / 2));
            float rightZ = sin(camYaw + (3.1415 / 2));

            camData[0][2] -= rightZ * walkSpeed;
            camData[0][0] -= rightX * walkSpeed;
        }
        if (window.keyboard[GLFW_KEY_W])
        {
            float forwardX = cos(camYaw);
            float forwardZ = sin(camYaw);

            camData[0][2] += forwardZ * walkSpeed;
            camData[0][0] += forwardX * walkSpeed;
        }
        if (window.keyboard[GLFW_KEY_S])
        {
            float forwardX = cos(camYaw);
            float forwardZ = sin(camYaw);

            camData[0][2] -= forwardZ * walkSpeed;
            camData[0][0] -= forwardX * walkSpeed;
        }

        if (window.keyboard[GLFW_KEY_LEFT])
        {
            camYaw += 0.1;
        }
        if (window.keyboard[GLFW_KEY_RIGHT])
        {
            camYaw -= 0.1;
        }

        if (window.keyboard[GLFW_KEY_UP])
        {
            camPitch += 0.1;
        }
        if (window.keyboard[GLFW_KEY_DOWN])
        {
            camPitch -= 0.1;
        }

        camData[1][0] = cos(camYaw) * cos(camPitch);
        camData[1][2] = sin(camYaw) * cos(camPitch);
        camData[1][1] = sin(camPitch);
        

        bgfx::setUniform(camUniform, camData, 2);
        
        compute.dispatch();

        bgfx::setState(
                BGFX_STATE_WRITE_R
                        | BGFX_STATE_WRITE_G
                        | BGFX_STATE_WRITE_B
                        | BGFX_STATE_WRITE_A
        );

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);
        
        bgfx::setTexture(0, tex, compute.getTexture());

        float textureSize[4];
        textureSize[0] = compute.textureWidth;
        textureSize[1] = compute.textureHeight;
        bgfx::setUniform(texSize, textureSize);

        bgfx::submit(0, program);

		

        window.finishRendering();
		
	}
	
    window.destroy();
    return EXIT_SUCCESS;
}