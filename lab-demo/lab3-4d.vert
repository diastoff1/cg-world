#version 150

uniform mat4 projectionMatrix; //lab2
uniform mat4 total; //lab2 too
uniform mat4 wToView; //2-4
uniform mat4 myMatrix; //rotation of the model -- not used

in vec3 in_Position;
in vec3 in_Normal;
in vec2 inTexCoord;

out vec3 out_Normal;
out vec2 out_TexCoord;


void main(void)
{
    //below I took of the rotation based on time
    //mat4 modelMat = total * myMatrix;
    mat4 modelMat = total;
    // In the line below I took out the mymatrix in order to things not move
    gl_Position = projectionMatrix * wToView * modelMat * vec4(in_Position, 1.0);
    // The 2 lines below 
    mat3 normalMatrix = mat3(modelMat);
    out_Normal = normalize(normalMatrix * in_Normal);
    out_TexCoord = inTexCoord;
}
