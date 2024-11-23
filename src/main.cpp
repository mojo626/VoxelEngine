#include <iostream>


#include <map>

#include "common.h"
#include "window.hpp"


#include "../res/shaders/basic/f_shader.fs.h"
#include "../res/shaders/basic/v_shader.vs.h"
#include "../res/shaders/basic/gol.cs.h"
#include "../res/shaders/basic/voxel.cs.h"

#include "ComputeManager.hpp"
#include "player.hpp"


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

    ComputeManager compute(512, 512, 128);

    Player player;
    
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

    

    float sizeData[4];
    sizeData[0] = 512;
    sizeData[1] = 512;

    

    bgfx::setUniform(sizeUniform, sizeData, 1);

    while (!glfwWindowShouldClose(window.getWindow())) {


        window.startRendering();

        player.update(&window);
        

        bgfx::setUniform(camUniform, player.getCamData().camData, 2);
        
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