#version 150

uniform mat4 projectionMatrix;
uniform mat4 wToView;
uniform mat4 total;

in vec3 in_Position;
in vec2 inTexCoord;

out vec2 out_TexCoord;

void main() {
    gl_Position = projectionMatrix * wToView * total * vec4(in_Position, 1.0);
    out_TexCoord = inTexCoord;
}
