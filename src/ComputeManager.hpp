#ifndef COMPUTE_MAN_H
#define COMPUTE_MAN_H

#include <vector>
#include "common.h"
#include <iostream>

//article about octrees
//https://developer.nvidia.com/gpugems/gpugems2/part-v-image-oriented-computing/chapter-37-octree-textures-gpu

class ComputeManager {
    public:
        int textureWidth;
        int textureHeight;
        bgfx::ProgramHandle computeProgram;
        bgfx::TextureHandle inputTexture;
        bgfx::TextureHandle outputTexture;

        ComputeManager(int textureWidth, int textureHeight) {


            //Setting up compute shader
            this->textureWidth = textureWidth;
            this->textureHeight = textureHeight;
        }

        void init()
        {
            // Create input texture with random data
            std::vector<float> inputTextureData(textureWidth * textureHeight * textureWidth * 4);
            for (uint32_t i = 0; i < inputTextureData.size(); i+=4) {
                int input = (rand() / (float)RAND_MAX) > 0.5f ? 1.0f : 0.0f;
                inputTextureData[i] = input;
                inputTextureData[i+1] = input;
                inputTextureData[i+2] = input;
                inputTextureData[i+3] = input;
            }

            std::cout << "3d texture data created" << std::endl;
            


            computeProgram = bgfx::createProgram( bgfx::createShader( bgfx::makeRef(voxel_cs, sizeof(voxel_cs)) ), true);

            // Create input texture (RGBA32F format)
            inputTexture = bgfx::createTexture3D(
                textureWidth,
                textureHeight,
                textureWidth,
                false,
                bgfx::TextureFormat::RGBA32F,
                BGFX_TEXTURE_COMPUTE_WRITE | BGFX_SAMPLER_POINT,
                bgfx::copy(inputTextureData.data(), inputTextureData.size() * sizeof(float))
            );

            

            // Create output texture (empty, same size as input)
            outputTexture = bgfx::createTexture2D(
                textureWidth,
                textureHeight,
                false,
                1,
                bgfx::TextureFormat::RGBA32F,
                BGFX_TEXTURE_COMPUTE_WRITE | BGFX_SAMPLER_POINT
            );
        }

        // void setPixel(int x, int y)
        // {
        //     std::vector<float> textureData(4);

        //     textureData[0] = 1.0f;
        //     textureData[1] = 1.0f;
        //     textureData[2] = 1.0f;
        //     textureData[3] = 1.0f;
            
        //     // Update the existing texture with the new data.
        //     bgfx::updateTexture2D(
        //         inputTexture,                       // Texture handle
        //         1,
        //         0,                                  // Mip level
        //         x,                                  // X offset
        //         y,                                  // Y offset
        //         1,                        // Width
        //         1,                       // Height
        //         bgfx::copy(textureData.data(), textureData.size() * sizeof(float))
        //     );
            
        // }

        void dispatch() {

            
            if (!bgfx::isValid(inputTexture) || !bgfx::isValid(outputTexture)) {
                std::cerr << "Invalid texture handle!" << std::endl;
                return;
            }
            // Bind the input and output textures to the compute shader
            
            bgfx::setImage(0, inputTexture, 0, bgfx::Access::Read);
            bgfx::setImage(1, outputTexture, 0, bgfx::Access::ReadWrite);
            

            bgfx::dispatch(0, computeProgram, textureWidth, textureHeight, 1);


            std::vector<float> outputTextureData(textureWidth * textureHeight * 4);
            bgfx::readTexture(outputTexture, outputTextureData.data());

            // for (size_t i = 0; i < 10; ++i) { // Print a few pixels for verification
            //     std::cout << "Pixel " << i << ": "
            //             << outputTextureData[i * 4 + 0] << ", "  // R
            //             << outputTextureData[i * 4 + 1] << ", "  // G
            //             << outputTextureData[i * 4 + 2] << ", "  // B
            //             << outputTextureData[i * 4 + 3] << "\n"; // A
            // }
        }

        bgfx::TextureHandle getTexture() {
            


            bgfx::TextureHandle returnTex = outputTexture;

            //bx::swap(inputTexture, outputTexture);
            
            outputTexture = bgfx::createTexture2D(
                textureWidth,
                textureHeight,
                false,
                1,
                bgfx::TextureFormat::RGBA32F,
                BGFX_TEXTURE_COMPUTE_WRITE | BGFX_SAMPLER_POINT
            );

            return returnTex;
        }
};


#endif