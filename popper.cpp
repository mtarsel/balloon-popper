#include "shaders.h"
#include <unistd.h>
#include <cstdlib>
#include <ctime>
using namespace std;


GLuint program;

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

GLfloat randglfloat(float min, float max){
	return min+static_cast <GLfloat> (rand())/(static_cast <GLfloat> (RAND_MAX/(max-min)));
}

class drawobj {
public:
  GLuint vaoID,vboID[2],eboID;//buffer objects
  //vertex and color arrays and whatnot
  GLfloat* vertexarray;
  GLfloat* colorarray;
  GLubyte* elems;
  //sizes for the binding of the buffer
  int vertexsize;
  int colorsize;
  int elemssize;
  //for translations
  glm::vec3 objTran;
  //is this a moving object?
  int move;
  //what is the speed
  float speed;
  //scale
  GLfloat scale;
  //for setting the draw buffer to this object
  void setbuffers() {
	glGenVertexArrays(1,&vaoID);
    glBindVertexArray(vaoID);
	
    glBindBuffer(GL_ARRAY_BUFFER,vboID[0]);
    glBufferData(GL_ARRAY_BUFFER,vertexsize,vertexarray,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
	
    glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ARRAY_BUFFER,colorsize,colorarray,GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,elemssize,elems,GL_STATIC_DRAW);
  
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
	
	//this section handles the translation of the object
    glm::mat4 trans; //I know this seems wasteful but for some reason this needs to be declared here every time.
	trans=glm::translate(trans,objTran);//translate the object

    GLint tempLoc = glGetUniformLocation(program,"modelMatrix");//Matrix that handle the transformations
    glUniformMatrix4fv(tempLoc,1,GL_FALSE,&trans[0][0]);
  }
  void draw(){
	if(move == 1){
		if(objTran.x <= -30.5-vertexarray[3]){ //vertexarray[3] is added to compensate for scale
			objTran.x = 28.0+vertexarray[3];
		
			objTran.y = randglfloat(-27.0, 3.5);
		}
		else objTran.x -= speed;
	}
	glDrawElements(GL_POLYGON,elemssize,GL_UNSIGNED_BYTE,NULL);
  }
} ;

class drawarray{
public:
	int size;
	drawobj array[500];
	void init(){size=0;}
} objectarray;

drawobj arrow;

//because sizes of arrays aren't passed with the data in c++ -.- but I'd rather have a working function than a pretty function
void create_object(int vertsize, int colorsize, int elemssize, GLfloat tempvert[], GLfloat tempcolor[], GLubyte tempelems[], drawobj &targetobject, int move){
	targetobject.move = move;
	targetobject.vertexarray = (GLfloat*)malloc(sizeof(GLfloat)*vertsize);
	targetobject.vertexsize = vertsize;
	memcpy(targetobject.vertexarray, tempvert, 4*vertsize);

	targetobject.colorarray = (GLfloat*)malloc(sizeof(GLfloat)*colorsize);
	targetobject.colorsize = colorsize;
	memcpy(targetobject.colorarray, tempcolor, 4*colorsize);
	
	targetobject.elems = (GLubyte*)malloc(sizeof(GLfloat)*elemssize);
	targetobject.elemssize = elemssize;
	memcpy(targetobject.elems, tempelems, elemssize);
	
	targetobject.objTran.x = 28.0+targetobject.vertexarray[3]; //create the object on the right
	targetobject.objTran.y = randglfloat(-27.0, 3.5); //at a random y value
	
	//initial buffer creation
	glGenBuffers(2, targetobject.vboID);
	glGenBuffers(1,&targetobject.eboID);
}

void init_arrow(){
	GLfloat tempvert[] = {0.0f,0.0f,
						 0.0f,3.0f,
						 3.0f,1.5f};
						 
	GLfloat tempcolor[]={1.0f,0.0f,0.0f,1.0f,
					   1.0f,0.0f,0.0f,1.0f,
					   1.0f,0.0f,0.0f,1.0f};
					   
	GLubyte tempelems[]={0,1,2};
	
	create_object(sizeof(tempvert), sizeof(tempcolor), sizeof(tempelems), tempvert, tempcolor, tempelems, arrow, 0);
	//initial reposition of arrow, overriding the default create_object position.
	arrow.objTran.x = -22.0; arrow.objTran.y = -12.0;
}


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
		printf("hit front boundary\n");
			
			//cin.get();
			if(((-0.5)*(objectarray.array[i].objTran.x)+((arrow.objTran.y+3.0)-11.5) >= objectarray.array[i].objTran.y) && ((0.5)*(objectarray.array[i].objTran.x)+((arrow.objTran.y)+11.5) <= (objectarray.array[i].objTran.y+objectarray.array[i].scale))){
			//printf("cube %i -- point on line: %f, top face of cube: %f\n", i, ((0.5)*(objectarray.array[i].objTran.x)+(arrow.objTran.y)+11.5), (objectarray.array[i].objTran.y)+(objectarray.array[i].scale));
			//printf("cube -- x: %f y: %f\n", objectarray.array[i].objTran.x, objectarray.array[i].objTran.y);
			//printf("arrow -- x: %f y: %f\n", arrow.objTran.x, arrow.objTran.y);
			
				printf("inside tri bitch\n");
				sleep(5);
			}
		}
	}
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
