#version 330

in vec2 uv;
uniform mat4 texMatB;
uniform sampler2DRect texB;

vec4 sourceB()
{
    vec4 result = texture( texB, vec2( texMatB * vec4( uv, 0.0, 1.0 ) ) );
    return result;
}

