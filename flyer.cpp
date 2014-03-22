#include "shaders.h"
#include "drawobj.h"
#include "globals.h"

/*GLfloat pit,yaw,scalar=1;

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
SDL_Event lastkey;*/

class drawarray{
public:
	int size;
	drawobj array[500];
	void init(){size=0;}
} objectarray;


void init(){
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 800);
	program=initShaders(shaders);
	
	init_arrow();
}

void detectcollision(){
	int i;
	for(i=0;i<objectarray.size;i++){
		
		if((objectarray.array[i].objTran.x-2.5<=arrow.objTran.x)&&((objectarray.array[i].objTran.x+objectarray.array[i].scale>=arrow.objTran.x))){
			
			//cin.get();
			if(((-0.5)*(objectarray.array[i].objTran.x)+((arrow.objTran.y+3.0)-12.0) >= objectarray.array[i].objTran.y) && ((0.5)*(objectarray.array[i].objTran.x)+((arrow.objTran.y)+12.0) <= (objectarray.array[i].objTran.y+objectarray.array[i].scale))){
			
				sleep(3);
				seconds = difftime(time(NULL), timer);
				printf("score: %.0f\n", seconds);
				exit(0);
			}
		}
	}
}


void newshape(){
	if(objectarray.size == 500)
		printf("Buffer full\n");
	printf("%i\n", objectarray.size);
	GLfloat scale = randglfloat(0.5, 6.0);
	GLfloat tempvert[] = {0.0f,0.0f,
				    0.0f,scale,
				    scale,scale,
				    scale,0.0f};
					
	int i;
	GLfloat tempcolor[16];
	for(i=0;i<4;i++){
		tempcolor[i]=tempcolor[i+4]=tempcolor[i+8]=tempcolor[i+12]=randglfloat(0.0, 1.0);
	}
					   
	GLubyte tempelems[]={0,1,2,3};
	
	create_object(sizeof(tempvert), sizeof(tempcolor), sizeof(tempelems), tempvert, tempcolor, tempelems, objectarray.array[objectarray.size], 1);
	objectarray.array[objectarray.size].speed = 1.0/scale;
	objectarray.array[objectarray.size].scale = scale;
	printf("%f\n", 1.0/scale);
	objectarray.size++;
}


void display(SDL_Window* screen){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	detectcollision();
	arrow.setbuffers();
	arrow.draw();
	int i;
	for(i=0; i<objectarray.size; i++){
		objectarray.array[i].setbuffers();
		objectarray.array[i].draw();
	}
	
    glFlush();
    SDL_GL_SwapWindow(screen);
}

void input(SDL_Window* screen){
	if(pressed){
		if(arrow.objTran.y <= -27) arrow.objTran.y = -27;
		else if(lastkey.key.keysym.sym == SDLK_DOWN) arrow.objTran.y -= 0.5;
		if(arrow.objTran.y >= 3.5) arrow.objTran.y = 3.5;
		else if(lastkey.key.keysym.sym == SDLK_UP) arrow.objTran.y += 0.5;
		else if(lastkey.key.keysym.sym == SDLK_RIGHT){
		}
	}
    while ( SDL_PollEvent(&event) )
    {
        switch (event.type)
        {
            case SDL_QUIT:
				seconds = difftime(time(NULL), timer);
				printf("score: %.0f\n", seconds);
				exit(0);
		                break;
            case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE){
					seconds = difftime(time(NULL), timer);
					printf("score: %.0f\n", seconds);
					exit(0);
				}
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
	//timer start
	timer = time(NULL);

	//For random movement of objects
	srand(time(NULL));
	//create object array
	objectarray.init();
	
	//SDL window and context management
    SDL_Window *window;
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE); 
	
    if(SDL_Init(SDL_INIT_VIDEO)<0){//initilizes the SDL video subsystem
	fprintf(stderr,"Unable to create window: %s\n", SDL_GetError());
	SDL_Quit();
	exit(1);//die on error
    }

	//create window
    window = SDL_CreateWindow(
	"Space Flyer", //Window title
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
    int prevsecond=0;
    while(true){
    	//shape creation
    	if(objectarray.size<500){ //make sure it won't overflow the buffer
    	if((int)difftime(time(NULL), timer)!=prevsecond){ //check to make sure it's the next second
    		if(objectarray.size<6){ //for the first 6 shapes
    			if((int)difftime(time(NULL), timer)%1==0) //make a new one every 5 seconds
    				newshape();
    		}
    		else{
    			if((int)difftime(time(NULL), timer)%15==0) //else, make one every 15
    				newshape();
    		}
    	}
    }
    prevsecond = (int)difftime(time(NULL), timer); //set the second to compare next iteration
    
	input(window);//keyboard controls
	display(window);//displaying
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
