/*	
Taken heavily from Perry Kivolowitz's Modern Hello World
specifically main.cpp
*/

#include <iostream>
#include <assert.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "background.h"
#include "stool.h"
#include "Sphere.h"
#include "world.h"
#include "cylinder.h"

using namespace std;
using namespace glm;

class Window
{
public:
	Window()
	{
		this->time_last_pause_began = this->total_time_paused = 0;
		this->normals = this->wireframe = this->paused = false;
		this->slices = 20;
		this->interval = 1000 / 120;
		this->window_handle = -1;
		this->stacks = 20;
		this->shader = 0;
	}

	float time_last_pause_began;
	float total_time_paused;
	bool paused , wireframe, normals;
	int window_handle;
	int interval;
	int slices;
	int stacks;
	int shader;
	ivec2 size;
	float window_aspect;
	vector<string> instructions;
} window;

Background background;
stool Stool, Stool2;
Sphere sphere, sphere2, sphere3, sphere4;
World world;
cylinder Cylinder;

static GLfloat xrot = 0.0f, yrot = 0.0f;
static GLfloat xdiff = 0.0f, ydiff = 0.0f;
GLfloat radius, xpos, ypos, zpos;
GLfloat angleH, angleV;
float PI = 3.1415926f;



int debug_mode = 1;

void DisplayInstructions()
{
	if (window.window_handle == -1)
		return;

	vector<string> * s = &window.instructions;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, window.size.x, 0, window.size.y, 1, 10);
	glViewport(0, 0, window.size.x, window.size.y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(10, 15 * s->size(), -5.5);
	glScaled(0.1, 0.1, 1.0);
	for (auto i = s->begin(); i < s->end(); ++i)
	{
		glPushMatrix();
		glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *) (*i).c_str());
		glPopMatrix();
		glTranslated(0, -150, 0);
	}
}

void CloseFunc()
{
	window.window_handle = -1;
	background.TakeDown();
	Stool.TakeDown();
	Stool2.TakeDown();
	sphere.TakeDown();
	sphere2.TakeDown();
	sphere3.TakeDown();
	sphere4.TakeDown();
	world.TakeDown();
}

void ReshapeFunc(int w, int h)
{
	if (h > 0)
	{
		window.size = ivec2(w, h);
		window.window_aspect = float(w) / float(h);
	}
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	float current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

	switch (c)
	{
	case 'n':
		//Stool.EnableNormals(window.normals = !window.normals);
		Stool2.EnableNormals(window.normals);
		sphere.EnableNormals(window.normals = !window.normals);
		break;

	case 'w':
		window.wireframe = !window.wireframe;
		break;

	case 'p':
		if (window.paused == true)
		{
			// Will be leaving paused state
			window.total_time_paused += (current_time - window.time_last_pause_began);
		}
		else
		{
			// Will be entering paused state
			window.time_last_pause_began = current_time;
		}
		window.paused = !window.paused;
		break;

	case 's':
		window.shader++;
		if(window.shader > 2)
			window.shader = 0;
		cout << window.shader;
		sphere.TakeDown();
		sphere.Initialize(window.slices, window.stacks, window.shader);
		break;
	case 'v':
		PI = 2 * PI;
		if(PI > 6.29)
			PI = 3.1415926;
		break;
	case 'x':
	case 27:
		glutLeaveMainLoop();
		return;
	}
}

void SpecialFunc(int c, int x, int y)
{
	switch (c)
	{
	case GLUT_KEY_PAGE_UP:
		++window.slices;
		++window.stacks;
		Stool.TakeDown();
		Stool.Initialize(window.slices, window.stacks, window.shader);
		Stool2.TakeDown();
		Stool2.Initialize(window.slices, window.stacks, window.shader);
		sphere.TakeDown();
		sphere.Initialize(window.slices, window.stacks, window.shader);
		break;

	case GLUT_KEY_PAGE_DOWN:
		if (window.slices > 1)
		{
			--window.slices;
			window.slices = clamp(window.slices, 2, 50);
			--window.stacks;
			window.stacks = clamp(window.stacks, 2, 50);
			if (window.stacks < 1)
				window.stacks = 1;
			Stool.TakeDown();
			Stool.Initialize(window.slices, window.stacks, window.shader);
			Stool2.TakeDown();
			Stool2.Initialize(window.slices, window.stacks, window.shader);
			sphere.TakeDown();
			sphere.Initialize(window.slices, window.stacks, window.shader);

		}
		break;

	case GLUT_KEY_F1:
		debug_mode++;
		if(debug_mode > 2)
			debug_mode = 0;
		break;

	case GLUT_KEY_UP: 
			if (angleH < 89) 
			{
				angleH = angleH + 1.0f;	
				ypos = sin(angleH * (PI / 180)) * radius;
				zpos = -  cos(angleH * (PI / 180)) * -cos(angleV * (PI /180)) * radius; 
				xpos =  -cos(angleH * PI / 180) * -sin(angleV * PI /180) * radius;
			}
			break;

	case GLUT_KEY_DOWN:
		if (angleH > -89)
		{
			angleH = angleH - 1.0f;
			ypos = sin(angleH * (PI/ 180)) * radius;
			zpos = -cos(angleH * (PI / 180)) * -cos(angleV * (PI/180)) * radius;
			xpos = -cos(angleH * (PI / 180)) * -sin(angleV * (PI / 180)) * radius;
		}
		break;

	case GLUT_KEY_RIGHT: 
			angleV = angleV + 1.0f;
			ypos = ypos;
			xpos = sin(angleV * (PI / 180)) * cos(angleH * (PI / 180)) * radius;
			zpos = cos(angleV * (PI / 180)) * cos(angleH * (PI / 180)) * radius;
			break;
	case GLUT_KEY_LEFT:
			angleV = angleV - 1.0f;
			ypos = ypos;
			xpos = sin(angleV * (PI / 180)) * cos(angleH * (PI / 180)) * radius;
			zpos = cos(angleV * (PI / 180)) * cos(angleH * (PI / 180)) * radius;
			break;
	}
	return;

}

void DisplayFunc()
{
	float current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

	glEnable(GL_CULL_FACE);
	
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.size.x, window.size.y);

	if(debug_mode == 0)
		background.Draw(window.size);
	mat4 projection = perspective(25.0f, window.window_aspect, 1.0f, 100.0f);
	//if(debug_mode == 0)
		radius = 20.0f;
	//if(debug_mode > 0)
		//radius = 10.0f;
	mat4 modelview = lookAt(vec3(xpos, ypos, zpos), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
	modelview = rotate(modelview, -90.0f, vec3(1.0f, 0.0f, 0.0f));
	modelview = translate(modelview, vec3(0.0f, -2.0f, 0.0f));
	modelview = rotate(modelview, xrot, vec3(1.0f, 0.0f, 0.0f));
	modelview = rotate(modelview, yrot, vec3(0.0f, 1.0f, 0.0f));
	world.Draw(projection, modelview, window.size, xrot, yrot, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);
	modelview = rotate(modelview, 90.0f, vec3(1.0f, 0.0f, 0.0f));
	modelview = translate(modelview, vec3(0.0f, 2.0f, 0.0f));
	if(debug_mode == 2)
		sphere.Draw(projection, modelview, window.size, xrot, yrot, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);
	if (debug_mode < 2)
		Stool.Draw(projection, modelview, window.size, xrot, yrot, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);
	modelview = translate(modelview, vec3(2.0f, 0.0f, 0.0f));
	
	if(debug_mode == 0)
	{
		Stool2.Draw(projection, modelview, window.size, xrot, yrot, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);
		vec3 location;
		modelview = translate(modelview, vec3(-4.0f, 0.0f, 0.0f));
		Stool2.Draw(projection, modelview, window.size, xrot, yrot, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);
		modelview =  translate(modelview, vec3(2.0f, 0.0f, 0.0f));

		//modelview = rotate(modelview, 90.0f, vec3(0.0f, 1.0f, 0.0f));

		float x = 0.0f, y = 0.0f, z = 0.0f;
		float theta = PI, phi = 0.0f;
		float slices = 20.0f;
		float stacks = 12.0f;
		
		float increment_slices = (PI) / float(slices);
		float increment_stacks = (PI) / float(stacks);
		float sphere_radius = 6.0f;
		for(int i = 0; i < slices; ++i)
		{
			for(int j = 0; j < stacks; ++j)
			{
				modelview = translate(modelview, -location);
				x = sphere_radius * cos(theta) * sin(phi);
				y = sphere_radius * sin(theta) * sin(phi);
				z = sphere_radius * cos(phi);
				location = vec3(x, y, z);
				modelview = translate(modelview, location);
				sphere.Draw(projection, modelview, window.size, xrot, yrot, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);
				theta += increment_stacks;
			}
			theta = PI;
			phi += increment_slices;
		}
		//Cylinder.Draw(projection, modelview, window.size, xrot, yrot, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);
	}
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	DisplayInstructions();
	glFlush();
}



void TimerFunc(int value)
{

	if (window.window_handle != -1)
	{
		glutTimerFunc(window.interval, TimerFunc, value);
		glutPostRedisplay();
	}
}

void funcMouse(GLint button, GLint state, GLint xPt, GLint yPt)
{
 xdiff = xPt - yrot;
 ydiff = -yPt + xrot;
}


void funcMotion(GLint xPt, GLint yPt)
{
 yrot = xPt - xdiff;
 xrot = yPt + ydiff;
}


int main(int argc, char * argv[])
{

	radius = 20.0f;
	xpos = 0.0f;
	ypos = 0.0f;
	zpos = radius;
	angleH = 0.0f;
	angleV = 0.0f;

	glutInit(&argc, argv);
	glutInitWindowSize(1024, 512);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);

	window.window_handle = glutCreateWindow("Program 2");
	glutReshapeFunc(ReshapeFunc);
	glutCloseFunc(CloseFunc);
	glutDisplayFunc(DisplayFunc);

	glutMouseFunc(funcMouse);
	glutMotionFunc(funcMotion); 
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
	
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutTimerFunc(window.interval, TimerFunc, 0);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);


	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return 0;
	}

	if (!background.Initialize())
		return 0;

	if (!Stool.Initialize(window.slices, window.stacks, window.shader))
		return 0;

	if (!Stool2.Initialize(window.slices, window.stacks, window.shader))
		return 0;

	if (!sphere.Initialize(window.slices, window.stacks, window.shader))
		return 0;

	if (!sphere2.Initialize(window.slices, window.stacks, window.shader))
		return 0;

	if (!sphere3.Initialize(window.slices, window.stacks, window.shader))
		return 0;

	if (!sphere4.Initialize(window.slices, window.stacks, window.shader))
		return 0;

	if(!world.Initialize(window.slices))
		return 0;

	//if(!Cylinder.Initialize(window.slices, window.stacks, window.shader));
	//	return 0;

	glutMainLoop();
}
