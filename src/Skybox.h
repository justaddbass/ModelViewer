#ifndef EE_SKYBOX_H
#define EE_SKYBOX_H

#ifdef __APPLE__
    #include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
#endif
#include <vector>
#include "Shaders.h"

class Skybox {
public:
    Skybox(std::vector<const char*> files);
    void Draw();
    GLuint getShader() {return shader;}
private:
    GLuint textureID, vao, vbo, shader;
    static GLfloat skyboxVertices[];

};

#endif
