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



struct Ray
{
    vec3 origin;
    vec3 dir;
    vec3 invDir;
};

// Thanks to https://tavianator.com/2011/ray_box.html
float RayBoundingBoxDst(Ray ray, vec3 boxMin, vec3 boxMax)
{
    vec3 tMin = (boxMin - ray.origin) * ray.invDir;
    vec3 tMax = (boxMax - ray.origin) * ray.invDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    bool hit = tFar >= tNear && tFar > 0;
    float dst = hit ? tNear > 0 ? tNear : 0 : -1.0;
    return dst;
}

vec4 tree_lookup(Ray ray) // Lookup coordinates
{
    int texSize = 32;
    int index = 0;
    int depth = 0;

    vec3 currentPos;

    if (RayBoundingBoxDst(ray, vec3(0.0), vec3(32.0)) == -1.0) {
        return vec4(1.0);
    }
 
    while (true) {
        int x = (index % texSize) * 2;
        int y = ((index / texSize) % texSize) * 2;
        int z = (index / (texSize * texSize)) * 2;

        float closestDis = 10000.0;
        int closestIndex = -1;

        for (int i = 0; i < 8; i++) {
            int offsetX = (i & 1) ? 1 : 0;
            int offsetY = (i & 2) ? 1 : 0;
            int offsetZ = (i & 4) ? 1 : 0;

            float dis = RayBoundingBoxDst(ray, currentPos + vec3(32.0 / pow(2, depth + 1)) * vec3(offsetX, offsetY, offsetZ), currentPos + vec3(32.0 / pow(2, depth + 1)) + vec3(32.0 / pow(2, depth + 1)) * vec3(offsetX, offsetY, offsetZ));

            vec4 img = imageLoad(inputTexture, vec3(x + offsetX, y + offsetY, z + offsetZ));

            if (dis != -1.0 && dis < closestDis && img.w != 0.0) {
                closestIndex = i;
                closestDis = dis;
            }
        }

        if (closestIndex == -1) {
            return vec4(depth/10.0);
        }

        int offsetX = (closestIndex & 1) ? 1 : 0;
        int offsetY = (closestIndex & 2) ? 1 : 0;
        int offsetZ = (closestIndex & 4) ? 1 : 0;
        vec4 img = imageLoad(inputTexture, vec3(x + offsetX, y + offsetY, z + offsetZ));

        if (img.w == 1.0) {
            return img;
        }

        index = img.z * 32 * 32 * 32 + img.y * 32 * 32 + img.x * 32;
        depth ++;

        currentPos += vec3(32.0 / pow(2, depth)) * vec3(offsetX, offsetY, offsetZ);

    }

    
    return vec4(0.0,1.0,0.0,1.0);
}


void main()
{
    

    // Here you would typically do something with the textures.
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    vec3 right = cross(vec3(0.0, 1.0, 0.0), camData[1].xyz);
    vec3 up = cross(camData[1].xyz, right);


    //vec3 clipPos = vec3((coord.xy/outputSize.xy)*2 - 1, nearClip);
    vec3 clipPos = ((coord.x/outputSize.x)*2 - 1) * right + ((coord.y/outputSize.y)*2 - 1) * up + nearClip * camData[1].xyz;

    vec3 rayDir = normalize(clipPos);

    Ray ray;
    ray.origin = camData[0].xyz;
    ray.dir = rayDir;
    ray.invDir = 1.0 / rayDir;

    vec4 color = tree_lookup(ray);

    imageStore(outputTexture, coord, color);
    // float step = 0.05;
    // float maxDis = 50.0;

    // for (float i = 0; i < maxDis; i += step) {
    //     vec3 rayPos = camData[0].xyz + (rayDir * i);

    //     if (imageLoad(inputTexture, rayPos).w > 0.0) {
    //         imageStore(outputTexture, coord, imageLoad(inputTexture, rayPos));
    //         return;
    //     }
    // }




    //imageStore(outputTexture, coord, vec4(0.0, 0.0, 0.0, 1.0));
}