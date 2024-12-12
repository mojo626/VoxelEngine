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
    bool isEmpty = true;          // Whether the node is empty
    bool isLeaf = false;          // Whether the node is a leaf node
    float rgba[4] = {0, 0, 0, 0}; // RGBA values (only meaningful for leaf nodes)
    std::unique_ptr<Node> children[8]; // Pointers to child nodes (nullptr if not present)
};

struct Texture3D {
    int size;
    std::vector<float> data; // RGBA values stored in a flat array
    int index = 0;

    Texture3D(int size) : size(size), data(size * size * size * 4, 0.0f) {}

    void setVoxel(int x, int y, int z, const float* rgba) {
        int index = ((z * size + y) * size + x) * 4;
        std::memcpy(&data[index], rgba, 4 * sizeof(float));
    }
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

                        auto perlinVal = db::perlin(x / (float)tex3dSize, y / (float)tex3dSize, z / (float)tex3dSize) / 2.0 + 0.5;
                        float input = (perlinVal) > 0.4 ? 1.0 : 0.0;

                        float r = (rand() / (float)RAND_MAX);
                        float g = (rand() / (float)RAND_MAX);
                        float b = (rand() / (float)RAND_MAX);
                        voxelData[index] = 0.0;
                        voxelData[index+1] = 0.0;
                        voxelData[index+2] = 1.0;
                        voxelData[index+3] = input;
                    }
                }   
            }

            std::vector<float> inputTextureData = voxelData;

            

            auto node = voxelGridToNode(voxelData);

            Texture3D tex = convertOctreeToTexture(node.get(), tex3dSize);

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
                bgfx::copy(tex.data.data(), tex.data.size() * sizeof(float))
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
        void encodeNodeToTexture(Node* node, Texture3D* texture) {
            
            int index = texture->index;

            int texSize = texture->size;
            
            int x = (index % texSize) * 2;
            int y = ((index / texSize) % texSize) * 2;
            int z = (index / (texSize * texSize)) * 2;

            texture->index ++;

            for (int i = 0; i < 8; i++)
            {
                int offsetX = (i & 1) ? 1 : 0;
                int offsetY = (i & 2) ? 1 : 0;
                int offsetZ = (i & 4) ? 1 : 0;

                if (node->children[i]->isEmpty) {
                    float rgba[4] = {0.0, 0.0, 0.0, 0.0};
                    texture->setVoxel(x + offsetX, y + offsetY, z + offsetZ, rgba);
                } else if (node->children[i]->isLeaf) {
                    float rgba[4] = {node->children[i]->rgba[0], node->children[i]->rgba[1], node->children[i]->rgba[2], 1.0};
                    texture->setVoxel(x + offsetX, y + offsetY, z + offsetZ, rgba);
                } else {
                    int newX = ((index + 1) % texSize);
                    int newY = (((index + 1) / texSize) % texSize);
                    int newZ = ((index + 1) / (texSize * texSize));

                    float rgba[4] = {newX / (float)texSize, newY / (float)texSize, newZ / (float)texSize, 0.5};
                    texture->setVoxel(x + offsetX, y + offsetY, z + offsetZ, rgba);

                    encodeNodeToTexture(node->children[i].get(), texture);
                }
            }

        }

        Texture3D convertOctreeToTexture(Node* root, int textureSize) {
            Texture3D texture(textureSize);
            encodeNodeToTexture(root, &texture);
            return texture;
        }

        bool areVoxelsUniform(const std::vector<float>& data) {
            std::vector<float> firstColor(&data[0], &data[4]);

            for (int i = 0; i < data.size(); i+=4) {
                std::vector<float> currentColor(&data[i], &data[i + 4]);

                for (int j = 0; j < 4; j++) {
                    if (currentColor[j] != firstColor[j]) {
                        return false;
                    }
                }
            }
            
            return true;
        }

        // Recursive function to convert a 3D grid into a voxel octree
        std::unique_ptr<Node> voxelGridToNode(const std::vector<float>& data) {
            int gridSize = pow(data.size() / 4, 1/3.0);

            auto node = std::make_unique<Node>();
            float color[4] = {0};

            //check if everything is the same within this area
            //(either everything is the same color or node is empty, this means this is a leaf node)
            //should also return if current node size is only one voxel
            if (areVoxelsUniform(data)) {
                node->isLeaf = true;
                node->isEmpty = data[3] == 0.0;

                for (int i = 0; i < 4; i++)
                {
                    node->rgba[i] = data[i];
                }
                return node;
            }

            int halfSize = gridSize / 2;


            for (int i = 0; i < 8; i++) {
                //use bitwise operations to get offsets from index
                int offsetX = (i & 1) ? halfSize : 0;
                int offsetY = (i & 2) ? halfSize : 0;
                int offsetZ = (i & 4) ? halfSize : 0;

                //generate new data that is just the new node
                std::vector<float> newData(halfSize * halfSize * halfSize * 4);
                for (int x = 0; x < halfSize; x++)
                {
                    for (int y = 0; y < halfSize; y++)
                    {
                        for (int z = 0; z < halfSize; z++)
                        {
                            int index = (z * halfSize * halfSize + y * halfSize + x) * 4;

                            int index2 = ((z + offsetZ) * gridSize * gridSize + (y + offsetY) * gridSize + x + offsetX) * 4;

                            newData[index] = data[index2];
                            newData[index + 1] = data[index2 + 1];
                            newData[index + 2] = data[index2 + 2];
                            newData[index + 3] = data[index2 + 3];
                        }
                    }
                }

                //recursively get children nodes
                node->children[i] = voxelGridToNode(newData);
                node->isEmpty = false;
                node->isLeaf = false;
            }

            
            return node;
        }
};


#endif