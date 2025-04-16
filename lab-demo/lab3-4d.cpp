// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with added encapsulation and an extra "ground" object.
// Note that the files "lab2-7.frag", "lab2-7.vert" and "maskros512.tga" are required.

#define MAIN
#include "LittleOBJLoader.h"
#include "GL_utilities.h"
#include "MicroGlut.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"
#include <math.h>

// Globals
GLuint myTex;
GLuint myTexSkybox;

Model *m1;
Model *m2;
Model *m4;
Model *m5;

mat4 originalWToView; //used to create a copy of the matrix wToView for the skybox within display
vec3 lightSourceColor = vec3(1.0f, 0.0f, 0.0f); // Red light
vec3 lightSourceDirection = vec3(0.7f, 0.0f, 0.7f); // Light along diagonal

// struct to handle the keyboard inputs
typedef struct{
    GLfloat z = 0.0f;
    GLfloat y = 0.0f;
    GLfloat x = 0.0f;
} Keyboard;
Keyboard keyboard_values;

#define near 1
#define far 500.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

// projection matrix
GLfloat projectionMatrix[] = {
    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
    0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
    0.0f, 0.0f, -(far+near)/(far-near), -2*far*near/(far-near),
    0.0f, 0.0f, -1.0f, 0.0f
};

// shaders
GLuint program;
GLuint skyboxProgram; 

// object 1 buffers (for m1)
GLuint bunnyVertexArrayObjID;
GLuint bunnyVertexBufferObjID;
GLuint bunnyIndexBufferObjID;
GLuint bunnyNormalBufferObjID;
GLuint bunnyTexCoordBufferObjID;

// object 2 buffers (for m2)
GLuint bunnyVertexArrayObjID2;
GLuint bunnyVertexBufferObjID2;
GLuint bunnyIndexBufferObjID2;
GLuint bunnyNormalBufferObjID2;
GLuint bunnyTexCoordBufferObjID2;

// initialize buffers for the ground model
GLuint groundVertexArrayObjID;
GLuint groundVertexBufferObjID;
GLuint groundNormalBufferObjID;
GLuint groundTexCoordBufferObjID;
GLuint groundIndexBufferObjID;

// initialize for the skybox
GLuint skyboxVertexArrayObjID;
GLuint skyboxVertexBufferObjID;
GLuint skyboxNormalBufferObjID;
GLuint skyboxTexCoordBufferObjID;
GLuint skyboxIndexBufferObjID;

// object 5 buffers (for m5)
GLuint klingonVertexArrayObjID;
GLuint klingonVertexBufferObjID;
GLuint klingonIndexBufferObjID;
GLuint klingonNormalBufferObjID;
GLuint klingonTexCoordBufferObjID;

// struct to describe a object to draw therefore.
typedef struct {
    Model *model;
    GLuint shader;
    GLuint texture;
    mat4 placement;
    GLuint vao;
    int numIndices;
} Object;

// number of objects to include m1, m2, and ground (till now but again who knows if I will update the comment).
#define NUM_OBJECTS 5
Object objects[NUM_OBJECTS];


// initialize shader program and set the projection matrix
GLuint InitShaderProgram(const char *vertFile, const char *fragFile, GLfloat *projMatrix) {
    GLuint prog = loadShaders(vertFile, fragFile);
    glUniformMatrix4fv(glGetUniformLocation(prog, "projectionMatrix"), 1, GL_TRUE, projMatrix);
    return prog;
}

// load texture and set the texture uniform
void InitTexture(const char *filename, GLuint *tex, GLuint prog) {
    LoadTGATextureSimple((char *)filename, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glUniform1i(glGetUniformLocation(prog, "texUnit"), 0);
}

// initialize buffers (VAO/VBO) for a given model
void InitModelBuffers(Model *m, GLuint prog,
                     GLuint *vao, GLuint *vertexBuffer, GLuint *normalBuffer,
                     GLuint *texCoordBuffer, GLuint *indexBuffer)
{
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    // get attribute locations
    GLint posLoc = glGetAttribLocation(prog, "in_Position");
    GLint normLoc = glGetAttribLocation(prog, "in_Normal");
    GLint texLoc = glGetAttribLocation(prog, "inTexCoord");

    // vertex positions 
    glGenBuffers(1, vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices * 3 * sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posLoc);

    // normals (only if shader uses them)
    if (normLoc != -1 && m->normalArray != NULL)
    {
        glGenBuffers(1, normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, *normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, m->numVertices * 3 * sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
        glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normLoc);
    }

    // texture coordinates 
    if (texLoc != -1 && m->texCoordArray != NULL)
    {
        glGenBuffers(1, texCoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, *texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, m->numVertices * 2 * sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
        glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(texLoc);
    }

    // index buffer 
    glGenBuffers(1, indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices * sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);
}

// draw an object
void DrawObject(Model *m, GLuint shader, GLuint texture, mat4 placement, GLuint vao, int numIndices)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, "total"), 1, GL_TRUE, placement.m);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0L);
}


// this function updates the camera and transformation matrices and (IMPORTANT) sends to the shader
void UpdateCameraMatrices(GLfloat t, GLuint prog, Keyboard keyboard_values)
{
    glUniform1f(glGetUniformLocation(prog, "time"), t);

    // we are computing camera position in a circular orbit with radius r
    float speed = t * 0.5f;
    float r = 5.0f;
    //float camXPos = sin(speed) * r;
    //float camZPos = cos(speed) * r;
    //without moving
    float camXPos = r;
    float camZPos = r;
    
    // here we implemented the camera movement based on the keyboard input
    // note that the camera keeps looking at the center, so in the project this would be a thing to pay attention
    originalWToView = lookAt(camXPos + keyboard_values.x, 1.0f + keyboard_values.y, camZPos + keyboard_values.z,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(prog, "wToView"), 1, GL_TRUE, originalWToView.m);

    // this is the rotation of a model (we may not be using but who knows if I will erase this comment so check)
    float angle = 0.5f * t * 3.1415f;
    GLfloat myMatrix[] = {
        cos(angle),  0.0f, -sin(angle), 0.0f,
        0.0f,        1.0f,  0.0f,       0.0f,
        sin(angle),  0.0f,  cos(angle), 0.0f,
        0.0f,        0.0f,  0.0f,       1.0f
    };
    glUniformMatrix4fv(glGetUniformLocation(prog, "myMatrix"), 1, GL_TRUE, myMatrix);
}

void init(void)
{   
    // load models from the folder
    m1 = LoadModel("bunnyplus.obj");
    m2 = LoadModel("cube.obj");
    m4 = LoadModel("skybox.obj");
    m5 = LoadModel("klingon.obj");

    // ground data
    #define kGroundSize 100.0f
    vec3 ground_vertices[] = {
        { -kGroundSize, 0.0f, -kGroundSize },
        { -kGroundSize, 0.0f,  kGroundSize },
        {  kGroundSize, 0.0f, -kGroundSize },
        {  kGroundSize, 0.0f,  kGroundSize }
    };
    vec3 ground_normals[] = {
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f }
    };
    vec2 ground_tex_coords[] = {
        { 0.0f, 0.0f },
        { 0.0f, 20.0f },
        { 20.0f, 0.0f },
        { 20.0f, 20.0f }
    };
    GLuint ground_indices[] = { 0, 1, 2, 1, 3, 2 };

    // create ground model 
    Model *groundModel = LoadDataToModel(ground_vertices, ground_normals, ground_tex_coords, NULL,
                                           ground_indices, 4, 6);

    dumpInfo();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glEnable(GL_DEPTH_TEST);
    printError("GL inits");

    // initialize shader program for all the shaders
    program = InitShaderProgram("lab3-4d.vert", "lab3-4d.frag", projectionMatrix);
    skyboxProgram = InitShaderProgram("skybox.vert", "skybox.frag", projectionMatrix);
    printError("init shader");

    // load texture and set texture unit uniform (used for all objects)
    InitTexture("maskros512.tga", &myTex, program);
    // texture for skybox
    InitTexture("Daylight-Box.tga", &myTexSkybox, skyboxProgram);
    

    // initialize buffers for model m1
    InitModelBuffers(m1, program, &bunnyVertexArrayObjID, &bunnyVertexBufferObjID,
                     &bunnyNormalBufferObjID, &bunnyTexCoordBufferObjID, &bunnyIndexBufferObjID);
    printError("init arrays for m1");

    // initialize buffers for model m2
    InitModelBuffers(m2, program, &bunnyVertexArrayObjID2, &bunnyVertexBufferObjID2,
                     &bunnyNormalBufferObjID2, &bunnyTexCoordBufferObjID2, &bunnyIndexBufferObjID2);
    printError("init arrays for m2");
    
    // initialize buffers for ground
    InitModelBuffers(groundModel, program, &groundVertexArrayObjID, &groundVertexBufferObjID,
                 &groundNormalBufferObjID, &groundTexCoordBufferObjID, &groundIndexBufferObjID);
    printError("init arrays for ground");
    
    // initialize buffers for skybox (m4)
    InitModelBuffers(m4, skyboxProgram, &skyboxVertexArrayObjID, &skyboxVertexBufferObjID,
                 &skyboxNormalBufferObjID, &skyboxTexCoordBufferObjID, &skyboxIndexBufferObjID);
    printError("init arrays for skybox (m4)");
    
    // Initialize buffers for model m5
    InitModelBuffers(m5, program, &klingonVertexArrayObjID, &klingonVertexBufferObjID,
                     &klingonNormalBufferObjID, &klingonTexCoordBufferObjID, &klingonIndexBufferObjID);
    printError("init arrays for m5");

    // setup objects array with placement transforms for skybox (first) and the rest of the models.
    objects[0].model      = m4;
    objects[0].shader     = skyboxProgram;
    objects[0].texture    = myTexSkybox;
    objects[0].placement  = S(100.0f, 100.0f, 100.0f);
    objects[0].vao        = skyboxVertexArrayObjID;
    objects[0].numIndices = m4->numIndices;
    
    objects[1].model      = m1;
    objects[1].shader     = program;
    objects[1].texture    = myTex;
    objects[1].placement  = T(-1.0f, 1.0f, -3.0f);
    objects[1].vao        = bunnyVertexArrayObjID;
    objects[1].numIndices = m1->numIndices;

    objects[2].model      = m2;
    objects[2].shader     = program;
    objects[2].texture    = myTex;
    objects[2].placement  = T(1.0f, 0.0f, -3.0f);
    objects[2].vao        = bunnyVertexArrayObjID2;
    objects[2].numIndices = m2->numIndices;

    objects[3].model      = groundModel;
    objects[3].shader     = program;
    objects[3].texture    = myTex;
    objects[3].placement  = T(0.0f,0.0f,0.0f);  
    objects[3].vao        = groundVertexArrayObjID;
    objects[3].numIndices = groundModel->numIndices;
    
    objects[4].model      = m5;
    objects[4].shader     = program;
    objects[4].texture    = myTex;
    objects[4].placement  = T(3.0f,0.0f,0.0f);  
    objects[4].vao        = klingonVertexArrayObjID;
    objects[4].numIndices = m5->numIndices;
}

void display(void)
{
    printError("pre display");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    // handle keyboard input
    if (glutKeyIsDown('w')) keyboard_values.z -= 0.1f;
    if (glutKeyIsDown('a')) keyboard_values.x -= 0.1f;
    if (glutKeyIsDown('s')) keyboard_values.z += 0.1f;
    if (glutKeyIsDown('d')) keyboard_values.x += 0.1f;
    if (glutKeyIsDown('u')) keyboard_values.y += 0.1f;
    if (glutKeyIsDown('p')) keyboard_values.y -= 0.1f;

    // ---- SKYBOX PART ----
    glUseProgram(skyboxProgram);
    glDisable(GL_DEPTH_TEST);

    // create skybox view matrix (no translation bc of the cast to mat3)
    mat4 skyboxView = mat4(mat3(originalWToView));
    
    // set skybox uniforms
    glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "wToView"), 1, GL_TRUE, skyboxView.m);
    
    // draw skybox
    DrawObject(objects[0].model, skyboxProgram, objects[0].texture,
               objects[0].placement, objects[0].vao, objects[0].numIndices);

    // ----- START OF MAIN PROGRAM ----
    glUseProgram(program);
    glEnable(GL_DEPTH_TEST);

    // set light uniforms
    glUniform3fv(glGetUniformLocation(program, "lightSourceDirection"), 1, (GLfloat*)&lightSourceDirection);
    glUniform3fv(glGetUniformLocation(program, "lightSourceColor"), 1, (GLfloat*)&lightSourceColor);

    // update matrices for main program
    UpdateCameraMatrices(t, program, keyboard_values);

    // draw other objects
    for (int i = 1; i < NUM_OBJECTS; i++) {
        DrawObject(objects[i].model, objects[i].shader, objects[i].texture,
                   objects[i].placement, objects[i].vao, objects[i].numIndices);
    }

    printError("display");
    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("GL3 white triangle example2");
    glutDisplayFunc(display);
    glutRepeatingTimer(10);
    init();
    glutMainLoop();
    return 0;
}

