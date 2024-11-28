#include "bgfx_compute.sh"


// Input texture, binding at slot 0
IMAGE3D_RO(inputTexture, rgba32f, 0);
// Output texture, binding at slot 1
IMAGE2D_RW(outputTexture, rgba32f, 1);

uniform vec4 camData[2];
//camData[0].xyz = cam position
//camData[1].xyz = cam direction

uniform vec4 outputSize;

#define nearClip 1.0

// float4 tree_lookup(vec3 point) // Lookup coordinates
// {
    
//     for (float i = 0; i < 100; i++)
//     { // fixed # of iterations
//         vec3 P;
//         // compute lookup coords. within current node
//         P = (MND + floor(0.5 + I.xyz * 255.0)) * invS;
//         // access indirection pool
//         if (I.w < 0.9) // already in a leaf?
//             I = (float4)tex3D(IndirPool, P); // no, continue to next depth
//         // compute pos within next depth grid
//         MND = MND * N;
//     }
//     return (I);
// }


void main()
{
    

    // Here you would typically do something with the textures.
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    vec3 right = cross(vec3(0.0, 1.0, 0.0), camData[1].xyz);
    vec3 up = cross(camData[1].xyz, right);


    //vec3 clipPos = vec3((coord.xy/outputSize.xy)*2 - 1, nearClip);
    vec3 clipPos = ((coord.x/outputSize.x)*2 - 1) * right + ((coord.y/outputSize.y)*2 - 1) * up + nearClip * camData[1].xyz;

    vec3 rayDir = normalize(clipPos);


    float step = 0.05;
    float maxDis = 50.0;

    for (float i = 0; i < maxDis; i += step) {
        vec3 rayPos = camData[0].xyz + (rayDir * i);

        if (imageLoad(inputTexture, rayPos).w == 1.0) {
            imageStore(outputTexture, coord, imageLoad(inputTexture, rayPos));
            return;
        }
    }




    imageStore(outputTexture, coord, vec4(0.0, 0.0, 0.0, 1.0));
}