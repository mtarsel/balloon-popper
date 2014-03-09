#include "shaders.h"
#include <unistd.h>
#include <cstdlib>
using namespace std;


GLuint vaoID,vboID[2],eboID;
GLuint program;

GLfloat pit,yaw,scalar=1;
glm::vec3 cubeTran;

GLfloat size=20;

//Variables for input function
bool pressed = false;
SDL_Event event;
SDL_Event lastkey;

/*GLfloat vertexarray[]={0.0f,15.0f,-15.0f,
			10.0f,-10.0f,-20.0f,
			-10.0f,-10.0f,-20.0f,
			0.0f,15.0f,-15.0f,
			10.0f,-10.0f,-20.0f,
			0.0f,15.0f,-15.0f,
			-10.0f,-10.0f,-20.0f};
*/
GLfloat vertexarray[]={0.0f,0.0f,0.0f,
					   0.0f,3.0f,0.0f,
					   3.0f,1.5f,0.0f};

/*GLfloat colorarray[]={
//3.0f,3.0f,3.0f,3.0f,

					0.5f,1.0f,1.0f,1.0f,
				    1.0f,0.5f,1.0f,1.0f,
				    1.0f,1.0f,0.5f,1.0f,
				    1.0f,1.0f,1.0f,1.0f,
				    0.5f,1.0f,1.0f,1.0f,
				    1.0f,0.5f,1.0f,1.0f,
                     1.0f,1.0f,0.5f,1.0f
};*/
GLfloat colorarray[]={
					1.0f,0.0f,0.0f,1.0f,
					1.0f,0.0f,0.0f,1.0f,
					1.0f,0.0f,0.0f,1.0f
};

											
GLubyte elems[]={0,1,2,3,7,4,5,6,
    	          7,3,0,4,5,6,2,1,
    		  0,1,5,4,7,3,2,6};


void init(){
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 800);
	
    glGenVertexArrays(1,&vaoID);
    glBindVertexArray(vaoID);
	
    glGenBuffers(2, vboID);
    glBindBuffer(GL_ARRAY_BUFFER,vboID[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertexarray),vertexarray,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	
    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(colorarray),colorarray,GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
  
    glGenBuffers(1,&eboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elems),elems,GL_STATIC_DRAW);

    ShaderInfo shaders[]={
	{ GL_VERTEX_SHADER , "vertexshader.glsl"},
	{ GL_FRAGMENT_SHADER , "fragmentshader.glsl"}, 
	{ GL_NONE , NULL} 
    };
		
    program=initShaders(shaders);
  
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
	
	//initial reposition
	cubeTran.x = -22.0; cubeTran.y = -12.0;
}


void display(SDL_Window* screen){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
    glm::mat4 trans;
	
    trans=glm::translate(trans,cubeTran);//translate the balloon
    //printf("pressed: %i\n", pressed);
    GLint tempLoc = glGetUniformLocation(program,"modelMatrix");//Matrix that handle the transformations
    glUniformMatrix4fv(tempLoc,1,GL_FALSE,&trans[0][0]);
	
    glDrawElements(GL_POLYGON,24,GL_UNSIGNED_BYTE,NULL);
    glFlush();
    SDL_GL_SwapWindow(screen);
}

void input(SDL_Window* screen){

	if(pressed){
		if(cubeTran.y <= -27) cubeTran.y = -27;
		else if(lastkey.key.keysym.sym == SDLK_DOWN) cubeTran.y -= 0.4;
		if(cubeTran.y >= 3.4) cubeTran.y = 3.4;
		else if(lastkey.key.keysym.sym == SDLK_UP) cubeTran.y += 0.4;	
	}
	printf("y: %f\n", cubeTran.y);
    while ( SDL_PollEvent(&event) )
    {
        switch (event.type)
        {
            case SDL_QUIT:
				exit(0);
                break;
            case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE) exit(0);
                pressed = true;
                break;
            case SDL_KEYUP:
                pressed = false;
                break;
            default:
                break;
        }
    }
	lastkey = event;
    }



int main(int argc, char **argv){
	
	//SDL window and context management
    SDL_Window *window;
	
    if(SDL_Init(SDL_INIT_VIDEO)<0){//initilizes the SDL video subsystem
	fprintf(stderr,"Unable to create window: %s\n", SDL_GetError());
	SDL_Quit();
	exit(1);//die on error
    }

	//create window
    window = SDL_CreateWindow(
	"Balloon Popper", //Window title
	SDL_WINDOWPOS_UNDEFINED, //initial x position
	SDL_WINDOWPOS_UNDEFINED, //initial y position
	800,							//width, in pixels
	500,							//height, in pixels
	SDL_WINDOW_OPENGL	//flags to be had
    );
	
	//check window creation
    if(window==NULL){
	fprintf(stderr,"Unable to create window: %s\n",SDL_GetError());
    }
	

	//creates opengl context associated with the window
	SDL_GLContext glcontext=SDL_GL_CreateContext(window);
	
	//initializes glew
  glewExperimental=GL_TRUE;
  if(glewInit()){
    fprintf(stderr, "Unable to initalize GLEW");
    exit(EXIT_FAILURE);
  }
  
    init();
	
    while(true){
	input(window);//keyboard controls
	display(window);//displaying
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
 
    return 0;
}
