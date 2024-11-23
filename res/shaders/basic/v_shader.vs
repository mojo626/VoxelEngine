$input a_position
$output v_color0, v_texCoord0

#include <bgfx_shader.sh>
 
void main()
{
    gl_Position = vec4(a_position.xy, 0.0, 1.0);
    v_color0 = vec4(a_position.x, a_position.y, 0.0, 1.0);
    v_texCoord0 = a_position.xy;
}