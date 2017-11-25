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
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <AntTweakBar.h>

#include "AssimpLoader.h"
#include "Mesh.h"
#include "Shaders.h"
#include "Skybox.h"

const GLfloat quad_vertices[] = {
	100.0f,	-0.5f,	100.0f,
	-100.0f,	-0.5f,	-100.0f,
	-100.0f,	-0.5f,	100.0f,
	-100.0f,	-0.5f,	-100.0f,
	100.0f,	-0.5f,	100.0f,
	100.0f,	-0.5f,	-100.0f
};

const GLfloat quad_uv[] = {
	-1.0f,	1.0f,
	-1.0f,	-1.0f,
	1.0f,	1.0f,
	1.0f,	-1.0f,
	1.0f,	1.0f,
	-1.0f,	-1.0f
};

void render() {

}

int main(int argc, char** argv) {
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
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

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

	glViewport(0,0,windowWidth,windowHeight);
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

	//Arguments
	Mesh model(argv[1]);
	float scale = atof(argv[2]);

	// AntTweakBar
	// macos needs core profile
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(windowWidth, windowHeight);
	TwBar* bar = TwNewBar("C++ Model Viewer");
	//TwDefine(" ControlBar position='12 12' size='100 100' ");
	TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &scale, " label='scale'");


	// camera and perspective matrix
    glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(1,1,1), glm::vec3(0,0,0), glm::vec3(0,1,0));

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

	//Ground
	GLuint groundVAO, groundVerticesVBO, groundUvVBO;
	glGenVertexArrays(1, &groundVAO);
	glBindVertexArray(groundVAO);
	glGenBuffers(1, &groundVerticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, groundVerticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glGenBuffers(1, &groundUvVBO);
	glBindBuffer(GL_ARRAY_BUFFER, groundUvVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_uv), quad_uv, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindVertexArray(0);

	// Depth buffer
	GLuint depthFBO;
	glGenFramebuffers(1, &depthFBO);
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Setting up shaders
    glUseProgram(shader);
    vpID = glGetUniformLocation(shader, "vp");
	vpID2 = glGetUniformLocation(sb.getShader(), "vp");
    glUniformMatrix4fv(vpID, 1, GL_FALSE, &vp_mat[0][0]);
	glUniform1f(glGetUniformLocation(shader, "scale"), scale);

	GLuint lightID = glGetUniformLocation(shader, "lightPos");


	viewID = glGetUniformLocation(shader, "viewPos");

    double phi = 0;
    const double PI = 3.14159265;

    SDL_Event sdlEvent;
    bool isRunning = true;

    while(isRunning) {
        SDL_PollEvent(&sdlEvent);
		TwEventSDL(&sdlEvent, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
        if(sdlEvent.type == SDL_QUIT) {
            isRunning = false;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 pos = glm::vec3(0.8*cos(phi), 0.2, 0.8*sin(phi));
		glm::vec3 lightPos = glm::vec3(cos(phi), sin(phi), 1.0);
        view = glm::lookAt(pos, glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 vp_mat = projection * view;

		glUniform3f(viewID, pos.x, pos.y, pos.z);
		glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

		glUniformMatrix4fv(vpID, 1, GL_FALSE, &vp_mat[0][0]);
		glUseProgram(shader);
        model.Draw(shader);
#if 0
		// render groundVAO
		glBindVertexArray(groundVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
#endif	
		//TODO: move skybox render call and disable depth buffer
		glUniformMatrix4fv(vpID2, 1, GL_FALSE, &vp_mat[0][0]);
		sb.Draw();

		//TweakBar
		TwDraw();

        SDL_GL_SwapWindow(g_window);
        if(phi >= 2 * PI)
            phi = 0;
        else
            phi += 0.01;
    }

	TwTerminate();
	SDL_Quit();
}
