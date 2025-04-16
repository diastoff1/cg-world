#version 150

uniform float time;
uniform sampler2D texUnit;
uniform vec3 lightSourceDirection;
uniform vec3 lightSourceColor;

in vec2 out_TexCoord;
in vec3 out_Normal;

out vec4 frag_Color;

void main(void) {
    vec3 norm = normalize(out_Normal);
    vec3 light = normalize(lightSourceDirection);
    float diffuse = max(dot(norm, light), 0.0);
    vec4 texColor = texture(texUnit, out_TexCoord);
    frag_Color = texColor * vec4(lightSourceColor * diffuse, 1.0);
}
