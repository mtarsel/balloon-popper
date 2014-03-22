#ifndef GLOBAL_H_
#define GLOBALS_H_

GLfloat pit,yaw,scalar=1;

GLfloat size=20;

time_t timer;
double score=0;
double seconds=0;

ShaderInfo shaders[]={
	{ GL_VERTEX_SHADER , "vertexshader.glsl"},
	{ GL_FRAGMENT_SHADER , "fragmentshader.glsl"}, 
	{ GL_NONE , NULL} 
    };

//Variables for input function
bool pressed = false;
SDL_Event event;
SDL_Event lastkey;

#endif
