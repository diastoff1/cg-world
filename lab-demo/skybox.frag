#version 150
uniform sampler2D texUnit;
in vec2 out_TexCoord;
out vec4 frag_Color;

void main() {
    frag_Color = texture(texUnit, out_TexCoord); 
}
