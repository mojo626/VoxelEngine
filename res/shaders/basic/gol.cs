#include "bgfx_compute.sh"


// Input texture, binding at slot 0
IMAGE2D_RO(inputTexture, rgba32f, 0);
// Output texture, binding at slot 1
IMAGE2D_RW(outputTexture, rgba32f, 1);



void main()
{
    

    // Here you would typically do something with the textures.
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    
    int sum = 0;

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            if (x == 0 && y == 0)
            {
                continue;
            }
            if (imageLoad(inputTexture, coord + ivec2(x, y)).x == 1.0)
            {
                sum ++;
            }
        }
    }

    if (sum < 2)
    {
        imageStore(outputTexture, coord, vec4(vec3(0.0), 1.0));
    } else if (sum > 3)
    {
        imageStore(outputTexture, coord, vec4(vec3(0.0), 1.0));
    } else if (sum == 3)
    {
        imageStore(outputTexture, coord, vec4(vec3(1.0), 1.0));
    } else
    {
        imageStore(outputTexture, coord, imageLoad(inputTexture, coord));
    }
    
  

    // // Example of reading from the input texture and writing to the output texture:
    // vec4 pixel = imageLoad(inputTexture, coord);  // Read from the input texture 


 

}