#version 330

in vec2 uv;
uniform mat4 texMatA;
uniform sampler2DRect texA;

vec4 sourceA()
{
    vec4 result = texture( texA, vec2( texMatA * vec4( uv, 0.0, 1.0 ) ) );
    return result;
}

