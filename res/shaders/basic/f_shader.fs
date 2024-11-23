$input v_color0, v_texCoord0



/*
 * Copyright 2011-2024 Branimir Karadzic. All rights reserved.
 *  License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include <bgfx_shader.sh>

SAMPLER2D(tex, 0);
uniform vec4 texSize;

void main()
{
	gl_FragColor = texture2D(tex, ((v_texCoord0 + 1)/2.0));
	//gl_FragColor = vec4((v_texCoord0 + 1)/2.0, 0.0, 1.0);
}