/** @file main.cpp
 *  @brief man code for <app name>
 *
 *	@author <author>
 *	@date <day-month> 2012
 */

#include "stdafx.h"
#include "utils.h"
#include "utils_shaders.h"

#include <math.h>

#define _USE_MATH_DEFINES

#include "SOIL.h"

#include <vector>
#include <ctime>

using namespace std;

#pragma region globals

///////////////////////////////////////////////////////////////////////////////
// globals
GLuint g_screenWidth, g_screenHeight;

float g_camPos[3];

// shaders:
GLuint g_shader;

//
// tod: add texture ids here:
//
GLuint g_textureID;

GLuint posBufferID = 0;
GLuint texBufferID = 0;
GLuint indexBufferID = 0;
GLuint elementCount = 0;

const float STRIPE_LEN = 17.0f;
const float STRIPE_H   = 0.25f;
const int STRIPE_STEPS = 64;

const float DX = STRIPE_LEN/(float)STRIPE_STEPS;
const float DU = 1.0f/(float)STRIPE_STEPS;

// time:
double g_appTime = 0.0;	// global app time in seconds

// menu:
#define MENU_EXIT 1
#define MENU_RESET 2

#pragma endregion


///////////////////////////////////////////////////////////////////////////////
// function prototypes
bool InitApp();
void CreateVertexBuffers();
void CleanUp();

// callbacks:
void ChangeSize(int w, int h);
void ProcessMenu(int option);

// keyboard:
void ProcessNormalKeys(unsigned char key, int x, int y);
void PressSpecialKey(int key, int x, int y);
void ReleaseSpecialKey(int key, int x, int y);

// mouse:
void ProcessMouse(int button, int state, int x, int y);
void ProcessMouseMotion(int x, int y);

void Idle();
void UpdateScene(double deltaSeconds);
void RenderScene();

///////////////////////////////////////////////////////////////////////////////
// entry point
int main(int argc, char **argv)
{
	// init GLUT
	glutInit(&argc, argv);
	// use depth buffer, double buffering and standard RGBA mode
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200,200);
	// standard resolution:
	glutInitWindowSize(640, 480);
	// title:
	glutCreateWindow("<add title>");

	// set callbacks:
	glutKeyboardFunc(ProcessNormalKeys);
	glutSpecialFunc(PressSpecialKey);
	glutSpecialUpFunc(ReleaseSpecialKey);
	glutMouseFunc(ProcessMouse);
	glutMotionFunc(ProcessMouseMotion);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutReshapeFunc(ChangeSize);
	
	// init OpenGL extensions (like shader support, framebuffer object, etc)
	if (utInitOpenGL20() == false) return 1;

	// init whole application:
	if (InitApp() == false)
	{
		utLOG_ERROR("cannot init application...");
		return 1;
	}

	// start rendering and animation:
	glutMainLoop();

	// finish:
	CleanUp();

	return(0);
}

#pragma region Init & CleanUp
///////////////////////////////////////////////////////////////////////////////
bool InitApp() 
{
	// create GLUT menu:
	int menu = glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Exit", MENU_EXIT);
    glutAddMenuEntry("Reset", MENU_RESET);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);

	srand((unsigned int)time(NULL));

	utLoadAndBuildShader("data\\buf.vert", "data\\buf.frag", &g_shader);
    
    glUniform1i(glGetUniformLocation(g_shader, "texture0"), 0);

    g_camPos[0] = 0.0f;
    g_camPos[1] = 0.0f;
    g_camPos[2] = 12.2f;

    //
    // generate texture:
    //

    g_textureID = SOIL_load_OGL_texture
        (
        "data\\stripes.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y 
        );

    CreateVertexBuffers();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void CreateVertexBuffers()
{    
    const int VERTS_X = STRIPE_STEPS+1;
    const int NUM_VERTS = (STRIPE_STEPS+1)*(2);

    const float Z_START = -STRIPE_STEPS * DX * 0.5f;
    const float Y_START = -STRIPE_H;

    float vPos[NUM_VERTS][3];  
    float vTex[NUM_VERTS][2]; 
    const GLuint posBufSizeInBytes = sizeof(float)*3*NUM_VERTS;
    const GLuint texBufSizeInBytes = sizeof(float)*2*NUM_VERTS;

	float angle = 0;
	float step = M_PI/8;
	float r = 4;
	//vPos
    for (int i = 0; i <= STRIPE_STEPS; i+=4)
    {	
		//s = (0,0,0), r = 4
		vPos[i][0] = r*sin(angle);
		vPos[i][1] = -1;
		vPos[i][2] = r*cos(angle);

		vPos[i+1][0] = r*sin(angle);
		vPos[i+1][1] = 1;
		vPos[i+1][2] = r*cos(angle);
		
		vPos[i+2][0] = r*sin(angle+step);
		vPos[i+2][1] = 1;
		vPos[i+2][2] = r*cos(angle+step);
		
		vPos[i+3][0] = r*sin(angle+step);
		vPos[i+3][1] = -1;
		vPos[i+3][2] = r*cos(angle+step);

		angle += step;
		/*
		//Versja RD
		vTex[i][0] = 0;
		vTex[i][1] = 0;

		vTex[i+1][0] = 0;
		vTex[i+1][1] = 1;
			
		vTex[i+2][0] = 1;
		vTex[i+2][1] = 0;
		
		vTex[i+3][0] = 1;
		vTex[i+3][1] = 1;
		*/
		
		vTex[i][0] = i*0.015625;
		vTex[i][1] = 0;

		vTex[i+1][0] = i*0.015625;
		vTex[i+1][1] = 1;
			
		vTex[i+2][0] = i*0.015625+0.0625;
		vTex[i+2][1] = 0;
		
		vTex[i+3][0] = i*0.015625+0.0625;
		vTex[i+3][1] = 1;
        
        // todo: create geometry data
        //
        // add first vertex
        // and second one...
    }

    vector<int> buf;
    // triangles
    for (int i = 0; i < STRIPE_STEPS; i+=4)
    {
		buf.push_back(i);
		buf.push_back(i+1);
		buf.push_back(i+2);
		buf.push_back(i+3);

		
		//0132
        //
        // todo: add indices
        //
    }

    elementCount = buf.size();

    //
    // buffer objects
    //
	glGenBuffers(1, &posBufferID); 
	glBindBuffer(GL_ARRAY_BUFFER, posBufferID); 
	glBufferData(GL_ARRAY_BUFFER, posBufSizeInBytes, vPos, GL_STATIC_DRAW); 
	glGenBuffers(1, &texBufferID); 
	glBindBuffer(GL_ARRAY_BUFFER, texBufferID); 
	glBufferData(GL_ARRAY_BUFFER, texBufSizeInBytes, vTex, GL_STATIC_DRAW); 
	// analogicznie dla texBufferID...
	glGenBuffers(1, &indexBufferID); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*elementCount, &buf[0], GL_STATIC_DRAW); 
	// _indeksy_ to adres do tablicy lub wektora int (np.: &indices[0]) 
	// iloœæ_indeksow = d³ugoœæ wektora (np. std::vector)
    // todo: create buffer objects...
}

///////////////////////////////////////////////////////////////////////////////
void CleanUp()
{
	utDeleteAllUsedShaders();
}

#pragma endregion

#pragma region Input

///////////////////////////////////////////////////////////////////////////////
void ChangeSize(int w, int h)
{
    g_screenWidth = w;
    g_screenHeight = h;

    double aspect = (double)w/(double)h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // setup projection matrix
    gluPerspective(45.0f, aspect, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMenu(int option)
{
	if (option == MENU_EXIT)
	{
		exit(0);
	}
	else if (option == MENU_RESET)
	{

	}
}

///////////////////////////////////////////////////////////////////////////////
void ProcessNormalKeys(unsigned char key, int x, int y) 
{
	if (key == 27) 
		exit(0);
	else if (key == ' ')
	{
		
	}
	else if (key == 'r')
	{
        
	}
}

///////////////////////////////////////////////////////////////////////////////
void PressSpecialKey(int key, int x, int y) 
{
    if (key == GLUT_KEY_UP)
        g_camPos[2] -= 0.1f;
    else if (key == GLUT_KEY_DOWN)
        g_camPos[2] += 0.1f;
}


///////////////////////////////////////////////////////////////////////////////
void ReleaseSpecialKey(int key, int x, int y) 
{
	
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMouse(int button, int state, int x, int y)
{
	
}

///////////////////////////////////////////////////////////////////////////////
void ProcessMouseMotion(int x, int y)
{
	
}
#pragma endregion

#pragma region Update & Render

///////////////////////////////////////////////////////////////////////////////
void Idle()
{
	static double lastDeltas[3] = { 0.0, 0.0, 0.0 };
	static const double REFRESH_TIME = 1.0/60.0;	// monitor with 60 Hz
	
	// in milisec
	int t = glutGet(GLUT_ELAPSED_TIME);
	double newTime = (double)t*0.001;

	double deltaTime = newTime - g_appTime;
	if (deltaTime < 0) deltaTime = 0.0;
    if (deltaTime > REFRESH_TIME) deltaTime = REFRESH_TIME;

	// average:
	deltaTime = (deltaTime + lastDeltas[0] + lastDeltas[1] + lastDeltas[2]) * 0.25;

	g_appTime = g_appTime+deltaTime;

	// call Update:
	UpdateScene(deltaTime);

	// render frame:
	RenderScene();

	// save delta:
	lastDeltas[0] = lastDeltas[1];
	lastDeltas[1] = lastDeltas[2];
	lastDeltas[2] = deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
void UpdateScene(double deltaTime) 
{
      
}

///////////////////////////////////////////////////////////////////////////////
void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup camera:
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	

	// render something
	glUseProgram(g_shader);

    glUniform1f(glGetUniformLocation(g_shader, "animParam"), (float)g_appTime);
	glUniform1f(glGetUniformLocation(g_shader, "diffusePower"), 0.9f);
	glUniform1f(glGetUniformLocation(g_shader, "specularPower"), 115.0f);

    // setup camera:
    gluLookAt(g_camPos[0], g_camPos[1], g_camPos[2], 
              0.0, 0.0, g_camPos[2]-2.0, 
              0.0, 1.0, 0.0);

    //
    // todo: draw something... 
    //
	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
	glBindBuffer(GL_ARRAY_BUFFER, posBufferID); 
	glVertexPointer(3, GL_FLOAT, 0, 0); 
	glBindBuffer(GL_ARRAY_BUFFER, texBufferID); 
	glTexCoordPointer(2, GL_FLOAT, 0, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID); 
	glDrawElements(GL_QUADS, elementCount, GL_UNSIGNED_INT, 0); 
	glDisableClientState(GL_VERTEX_ARRAY); 
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glUseProgram(0);

	// draw text & fps:
	glColor3f(0.0f,1.0f,0.0f);
	utSetOrthographicProjection(g_screenWidth, g_screenHeight);
		utCalculateAndPrintFps(10, 35);
		utDrawText2D(10,55, "Esc - Quit");
		utDrawText2D(10,70, "R - Reset");
	utResetPerspectiveProjection();

    //
    /*Sleep(10);
    glFinish();*/
    //

	glutSwapBuffers();
}

#pragma endregion
