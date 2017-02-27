#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#else
	#include <GL/glew.h>
#endif
#undef main
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <stdio.h>
#include <math.h>

#include "AssimpLoader.h"
#include "Mesh.h"
#include "Shaders.h"
#include "Skybox.h"

int main(int, char**) {
    SDL_Window* g_window;
    unsigned int windowHeight = 600, windowWidth = 800;
    SDL_GLContext _glContext;
	SDL_Event e;
    SDL_Init(SDL_INIT_EVERYTHING);

    //required to get OpenGL 4.1. this may change
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    g_window = SDL_CreateWindow("Modelviewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
    _glContext = SDL_GL_CreateContext(g_window);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

#ifndef __APPLE__
    glewExperimental = true;
    GLenum err = glewInit();
    if(glewInit() != err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(1);
    }
    else
        printf("%s\n", glGetString(GL_VERSION));
#else
    printf("%s\n", glGetString(GL_VERSION));
#endif

    //Mesh model("wt_teapot.obj");
	Mesh model("dragon.obj");

    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(5,5,5), glm::vec3(0,0,0), glm::vec3(0,1,0));

    glm::mat4 vp_mat = projection * view;

    GLuint shader;
	GLuint viewID, projectionID, vpID, vpID2, textureID;

    shader = LoadShaders("basic.vs", "basic.fs");

	//Skybox
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/right.jpg");
	faces.push_back("skybox/left.jpg");
	faces.push_back("skybox/top.jpg");
	faces.push_back("skybox/bottom.jpg");
	faces.push_back("skybox/back.jpg");
	faces.push_back("skybox/front.jpg");
	Skybox sb(faces);

    glUseProgram(shader);
    vpID = glGetUniformLocation(shader, "vp");
	vpID2 = glGetUniformLocation(sb.getShader(), "vp");
    glUniformMatrix4fv(vpID, 1, GL_FALSE, &vp_mat[0][0]);

	GLuint lightID = glGetUniformLocation(shader, "lightPos");
	glUniform3f(lightID, 1.0f, 1.0f, 1.0f);

	viewID = glGetUniformLocation(shader, "viewPos");

    double phi = 0;
    const double PI = 3.14159265;

    SDL_Event sdlEvent;
    bool isRunning = true;

    while(isRunning) {
        SDL_PollEvent(&sdlEvent);
        if(sdlEvent.type == SDL_QUIT) {
            isRunning = false;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 pos = glm::vec3(0.9*cos(phi), 0.75, 0.9*sin(phi));
        view = glm::lookAt(pos, glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 vp_mat = projection * view;

		glUniform3f(viewID, pos.x, pos.y, pos.z);

		glUniformMatrix4fv(vpID, 1, GL_FALSE, &vp_mat[0][0]);
		glUseProgram(shader);
        model.Draw(shader);

		glUniformMatrix4fv(vpID2, 1, GL_FALSE, &vp_mat[0][0]);
		sb.Draw();

        SDL_GL_SwapWindow(g_window);
        if(phi >= 2 * PI)
            phi = 0;
        else
            phi += 0.01;
    }
}
