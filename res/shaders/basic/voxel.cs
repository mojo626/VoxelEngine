#include "bgfx_compute.sh"


// Input texture, binding at slot 0
IMAGE3D_RO(inputTexture, rgba32f, 0);
// Output texture, binding at slot 1
IMAGE2D_RW(outputTexture, rgba32f, 1);

uniform vec4 camData[2];
//camData[0].xyz = cam position
//camData[1].xyz = cam direction


void main()
{
    

    // Here you would typically do something with the textures.
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);




    imageStore(outputTexture, coord, vec4(vec3(camData[1].xyz), 1.0));
}