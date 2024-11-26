#ifndef COMPUTE_MAN_H
#define COMPUTE_MAN_H

#include <vector>
#include "common.h"
#include <iostream>

//article about octrees
//https://developer.nvidia.com/gpugems/gpugems2/part-v-image-oriented-computing/chapter-37-octree-textures-gpu

/*



*/

struct Node {
    bool isEmpty; //are there child nodes?
    bool hasData; //is there color data (is this is leaf node)
    Node* A;
    Node* B;
    Node* C;
    Node* D;
    Node* E;
    Node* F;
    Node* G;
    Node* H;
};

class ComputeManager {
    public:
        int textureWidth;
        int textureHeight;
        int tex3dSize;
        bgfx::ProgramHandle computeProgram;
        bgfx::TextureHandle inputTexture;
        bgfx::TextureHandle outputTexture;

        ComputeManager(int textureWidth, int textureHeight, int tex3dSize) {


            //Setting up compute shader
            this->textureWidth = textureWidth;
            this->textureHeight = textureHeight;
            this->tex3dSize = tex3dSize;
        }

        void init()
        {
            std::vector<float> voxelData(tex3dSize * tex3dSize * tex3dSize * 4);

            for (int x = 0; x < tex3dSize; x++) {
                for (int y = 0; y < tex3dSize; y++) {
                    for (int z = 0; z < tex3dSize; z++) {
                        int index = (x + y * tex3dSize + z * tex3dSize * tex3dSize)*4;

                        int input = (rand() / (float)RAND_MAX) > 0.5f ? 1.0f : 0.0f;

                        float r = (rand() / (float)RAND_MAX);
                        float g = (rand() / (float)RAND_MAX);
                        float b = (rand() / (float)RAND_MAX);
                        voxelData[index] = r;
                        voxelData[index+1] = g;
                        voxelData[index+2] = b;
                        voxelData[index+3] = input;
                    }
                }   
            }

            std::vector<float> inputTextureData = voxelGridToSVO(voxelData);

            std::cout << "3d texture data created" << std::endl;
            


            computeProgram = bgfx::createProgram( bgfx::createShader( bgfx::makeRef(voxel_cs, sizeof(voxel_cs)) ), true);

            // Create input texture (RGBA32F format)
            inputTexture = bgfx::createTexture3D(
                tex3dSize,
                tex3dSize,
                tex3dSize,
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

        //data.size / 4 must be a power of 2
        std::vector<float> voxelGridToSVO(std::vector<float> data) {
            int maxDepth = log2( //convert size to max depth
                pow( //get side length
                    data.size() / 4.0, //divide by 4 since values are rgba
                    1/3.0)
                );

            // Create input texture with random data
            std::vector<float> inputTextureData(tex3dSize * tex3dSize * tex3dSize * 4);


            for (int i = 0; i < maxDepth; i++)
            {
                for (int x = 0; x < pow(2, i); x++) {
                    for (int y = 0; y < pow(2, i); y++) {
                        for (int z = 0; z < pow(2, i); z++) {

                        }
                    }
                }
            }

            for (uint32_t i = 0; i < data.size(); i+=4) {
                int index = i / 4;
                int x = index % tex3dSize;
                int y = (index / tex3dSize) % tex3dSize;
                int z = index / (tex3dSize * tex3dSize);

                //std::cout << x << ", " << y << ", " << z << std::endl;
            }

            return data;
        }

        Node voxelGridToNode(std::vector<float> data) {
            Node node;

            if (data.size() == 4) {
                node.isEmpty = true;
                if (data[3] == 1.0) {
                    node.hasData = true;
                } else {
                    node.hasData = false;
                }
                
                return node;
            }

            if (data.size() == 32) {
                
                node.A = &voxelGridToNode({data.begin(), data.begin() + 4});
                node.B = &voxelGridToNode(std::vector<float>{data[4]});
                node.C = &voxelGridToNode(std::vector<float>{data[8]});
                node.D = &voxelGridToNode(std::vector<float>{data[12]});
                node.E = &voxelGridToNode(std::vector<float>{data[16]});
                node.F = &voxelGridToNode(std::vector<float>{data[20]});
                node.G = &voxelGridToNode(std::vector<float>{data[24]});
                node.H = &voxelGridToNode(std::vector<float>{data[28]});
                node.isEmpty = false;
                node.hasData = false;
            }
        }
};


#endif