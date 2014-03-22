#ifndef DRAWARROW_H_
#define DRAWARROW_H_

#include "shaders.h"

GLuint program;

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

drawobj arrow;

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


#endif
