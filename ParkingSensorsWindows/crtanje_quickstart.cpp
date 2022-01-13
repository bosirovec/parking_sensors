#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>

#include "GL/glut.h"
#include "../../sndfile.hh"


#include <math.h>
#include <iostream>

#include <windows.h>
#include <mmsystem.h>
#include <cstdio>
#include <io.h>


#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <cstdlib>
#include<minmax.h>

#include<windows.h>

using namespace std;
void display(void);
void reshape(int, int);
void idle();


void turnOnSensors();
void draw();
int state[6] = { 0 };
float distances[6] = { 0 };
bool played;
POINT p;



#pragma region PreGivenCode


unsigned char* loadPPM(const char* filename, int& width, int& height) {
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}
	retval_fgets = fgets(buf[0], BUFSIZE, fp);
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}
	return rawData;
}

void initGL()
{
	glEnable(GL_TEXTURE_2D); // enable texture mapping
	glShadeModel(GL_SMOOTH); // enable smooth shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // get clear background (black color)
	glClearDepth(1.0f); // color depth buffer
	glDepthFunc(GL_LEQUAL); // configuration of depth testing
							//enable additional options regarding: perspective correction, anti-aliasing, etc
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void loadTexture()
{
	GLuint texture[1]; // declaring space for one texture
	int twidth, theight; // declaring variable for width and height of an image
	unsigned char* tdata; // declaring pixel data
						  // loading image data from specific file:
	tdata = loadPPM("..\\seat.ppm", twidth, theight);
	if (tdata == NULL) return; // check if image data is loaded
							   // generating a texture to show the image
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE,
		tdata);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//void readSensors(unsigned char key, int x, int y) {
//	switch (key) {
//	//	// Front Left
//	//case 'c':
//	//	state[0] = 3;
//	//	break;
//	//case 'x':
//	//	state[0] = 2;
//	//	break;
//	//case 'y':
//	//	state[0] = 1;
//	//	break;
//
//		//Front Right
//	//case 'd':
//	//	state[1] = 3;
//	//	break;
//	//case 's':
//	//	state[1] = 2;
//	//	break;
//	//case 'a':
//	//	state[1] = 1;
//	//	break;
//
//	//case 'e':
//	//	state[2] = 3;
//	//	break;
//	//case 'w':
//	//	state[2] = 2;
//	//	break;
//	//case 'q':
//	//	state[2] = 1;
//	//	break;
//
//
//	//	// Rear Left
//	//case 't':
//	//	state[3] = 3;
//	//	break;
//	//case 'z':
//	//	state[3] = 2;
//	//	break;
//	//case 'u':
//	//	state[3] = 1;
//	//	break;
//
//
//	//	// Rear Right
//	//case 'g':
//	//	state[4] = 3;
//	//	break;
//	//case 'h':
//	//	state[4] = 2;
//	//	break;
//	//case 'j':
//	//	state[4] = 1;
//	//	break;
//
//	//case 'b':
//	//	state[5] = 3;
//	//	break;
//	//case 'n':
//	//	state[5] = 2;
//	//	break;
//	//case 'm':
//	//	state[5] = 1;
//	//	break;
//
//	/*case 'r':
//		zvuk = 0;
//		for (int i = 0; i < 6; i++)
//			state[i] = 0;
//		display();
//		break;*/
//
//	/*case 'k':
//		state[0] = 0;
//		break;
//	case 'i':
//		state[1] = 0;
//		break;
//	case 'l':
//		state[2] = 0;
//		break;
//	case 'o':
//		state[3] = 0;
//		break;
//	*/}
//}

void display() {
	cerr << "display callback" << endl;
	// clean color buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// start drawing quads
	glBegin(GL_QUADS);
	// choose color (white)
	glColor3f(1, 1, 1);
	// coordinates of initial white rectangle for the background
	glTexCoord2f(0, 1); glVertex3f(-2, -1, 0);
	glTexCoord2f(1, 1); glVertex3f(2, -1, 0);
	glTexCoord2f(1, 0); glVertex3f(2, 1, 0);
	glTexCoord2f(0, 0); glVertex3f(-2, 1, 0);
	// end of drawing
	glEnd();


	// swap buffers to show new graphics
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	cerr << "reshape callback" << endl;
	// specify the desired rectangle
	glViewport(0, 0, width, height);
	// switch to matrix projection
	glMatrixMode(GL_PROJECTION);
	// clean projection matrix
	glLoadIdentity();
	// set camera view (orthographic projection with 4x4 unit square canvas)
	glOrtho(-2, 2, -2, 2, 2, -2);
	// swith back to matrix
	glMatrixMode(GL_MODELVIEW);
}

void idle()
{
	turnOnSensors();
	if (GetCursorPos(&p)) {}

}

#pragma endregion



#pragma region BornasCode
class Sensor {

public:
	int x;
	int y;

	Sensor(int xpos, int ypos) {
		x = xpos;
		y = ypos;
	}

	int getX() { return x; }
	int getY() { return y; }

};

void glDrawSensors() {
	//drawing sensor 0
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(-1.31f, -0.41f, 0.0f); // XYZ left, top
	glVertex3f(-1.27f, -0.41f, 0.0f); // XYZ right, top
	glVertex3f(-1.27f, -0.45f, 0.0f); // XYZ right, bottom
	glVertex3f(-1.31f, -0.45f, 0.0f); // XYZ left, bottom

	//drawing sensor 1
	glVertex3f(-1.42f, 0.02f, 0.0f); // XYZ left, top
	glVertex3f(-1.38f, 0.02f, 0.0f); // XYZ right, top
	glVertex3f(-1.38f, -0.02f, 0.0f); // XYZ right, bottom
	glVertex3f(-1.42f, -0.02f, 0.0f); // XYZ left, bottom

	//drawing sensor 2
	glVertex3f(-1.31f, 0.41f, 0.0f); // XYZ left, top
	glVertex3f(-1.27f, 0.41f, 0.0f); // XYZ right, top
	glVertex3f(-1.27f, 0.45f, 0.0f); // XYZ right, bottom
	glVertex3f(-1.31f, 0.45f, 0.0f); // XYZ left, bottom

	//drawing sensor 3
	glVertex3f(1.27f, 0.41f, 0.0f); // XYZ left, top
	glVertex3f(1.23f, 0.41f, 0.0f); // XYZ right, top
	glVertex3f(1.23f, 0.45f, 0.0f); // XYZ right, bottom
	glVertex3f(1.27f, 0.45f, 0.0f); // XYZ left, bottom


	//drawing sensor 4
	glVertex3f(1.37f, 0.02f, 0.0f); // XYZ left, top
	glVertex3f(1.33f, 0.02f, 0.0f); // XYZ right, top
	glVertex3f(1.33f, -0.02f, 0.0f); // XYZ right, bottom
	glVertex3f(1.37f, -0.02f, 0.0f); // XYZ left, bottom

	//drawing sensor 5
	glVertex3f(1.27f, -0.41f, 0.0f); // XYZ left, top
	glVertex3f(1.23f, -0.41f, 0.0f); // XYZ right, top
	glVertex3f(1.23f, -0.45f, 0.0f); // XYZ right, bottom
	glVertex3f(1.27f, -0.45f, 0.0f); // XYZ left, bottom
	return;
};


float distance(int x1, int y1, Sensor Sensor)
{
	int x2, y2;
	x2 = Sensor.x;
	y2 = Sensor.y;
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}


//Front Sensors Initialization
Sensor frontLeftSensor(680, 647);
Sensor frontMidSensor(660, 570);
Sensor frontRightSensor(680, 493);

//Rear Sensors Initialization
Sensor rearRightSensor(1135, 493);
Sensor rearMidSensor(1155, 570);
Sensor rearLeftSensor(1135, 647);

void turnOnSensors() {
	
	//Calculate Cursor Distance to Each Sensor
	distances[0] = distance(p.x, p.y, frontLeftSensor);
	distances[1] = distance(p.x, p.y, frontMidSensor);
	distances[2] = distance(p.x, p.y, frontRightSensor);
	distances[3] = distance(p.x, p.y, rearRightSensor);
	distances[4] = distance(p.x, p.y, rearMidSensor);
	distances[5] = distance(p.x, p.y, rearLeftSensor);

	//frontLeftSenSor Managment
	if (distances[0] <= 85) {
		state[0] = 1;
		if (distances[0] <= 65) {
			state[0] = 2;
			if (distances[0] <= 45) {
				state[0] = 3;
			}
		}
	}
	else { state[0] = 0; }

	//frontMidSensor Managment
	if (distances[1] <= 80) {
		state[1] = 1;
		if (distances[1] <= 60) {
			state[1] = 2;
			if (distances[1] <= 40) {
				state[1] = 3;
			}
		}
	}
	else { state[1] = 0; }

	//frontRightSensor Managment
	if (distances[2] <= 85) {
		state[2] = 1;
		if (distances[2] <= 65) {
			state[2] = 2;
			if (distances[2] <= 45) {
				state[2] = 3;
			}
		}
	}
	else { state[2] = 0; }

	//rearRightSensor Managment
	if (distances[3] <= 85) {
		state[3] = 1;
		if (distances[3] <= 65) {
			state[3] = 2;
			if (distances[3] <= 45) {
				state[3] = 3;
			}
		}
	}
	else { state[3] = 0; }

	//rearMidSensor Managment
	if (distances[4] <= 80) {
		state[4] = 1;
		if (distances[4] <= 60) {
			state[4] = 2;
			if (distances[4] <= 40) {
				state[4] = 3;
			}
		}
	}
	else { state[4] = 0; }

	//rearLeftSensor Managment
	if (distances[5] <= 85) {
		state[5] = 1;
		if (distances[5] <= 65) {
			state[5] = 2;
			if (distances[5] <= 45) {
				state[5] = 3;
			}
		}
	}
	else { state[5] = 0; }

	cerr << state[0] << "." << state[1] << "." << state[2] << "." << state[3] << "." << state[4] << "." << state[5] << endl;
	//cerr << "p.x: " << p.x << "  p.y: " << p.y << endl;

	draw();
	return;
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_QUADS);


	// drawing a background white rectangle
	glColor3f(1, 1, 1); // choosing a color
	glTexCoord2f(0, 1); glVertex3f(-2, -1, 0);
	glTexCoord2f(1, 1); glVertex3f(2, -1, 0);
	glTexCoord2f(1, 0); glVertex3f(2, 1, 0);
	glTexCoord2f(0, 0); glVertex3f(-2, 1, 0);

	glDrawSensors();

	switch (state[0]) {
		// Front Left
	case 3:
		// drawing a rectangle
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(-1.57f, -0.45f, 0.0f);
		glVertex3f(-1.50f, -0.41f, 0.0f);
		glVertex3f(-1.38f, -0.61f, 0.0f);
		glVertex3f(-1.44f, -0.65f, 0.0f);

		// drawing a rectangle
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-1.70f, -0.45f, 0.00f);
		glVertex3f(-1.63f, -0.41f, 0.0f);
		glVertex3f(-1.44f, -0.71f, 0.00f);
		glVertex3f(-1.50f, -0.75f, 0.0f);

		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.83f, -0.46f, 0.0f);
		glVertex3f(-1.75f, -0.41f, 0.0f);
		glVertex3f(-1.49f, -0.80f, 0.0f);
		glVertex3f(-1.57f, -0.85f, 0.0f);

		if(state[0]>=state[1])
			PlaySound(TEXT("D:\\left_3.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;

	case 2:
		// drawing a rectangle
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-1.70f, -0.45f, 0.00f);
		glVertex3f(-1.63f, -0.41f, 0.0f);
		glVertex3f(-1.44f, -0.71f, 0.00f);
		glVertex3f(-1.50f, -0.75f, 0.0f);
		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.83f, -0.46f, 0.0f);
		glVertex3f(-1.75f, -0.41f, 0.0f);
		glVertex3f(-1.49f, -0.80f, 0.0f);
		glVertex3f(-1.57f, -0.85f, 0.0f);
		if(state[0]>=state[1])
			PlaySound(TEXT("D:\\left_2.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;

	case 1:
		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.83f, -0.46f, 0.0f);
		glVertex3f(-1.75f, -0.41f, 0.0f);
		glVertex3f(-1.49f, -0.80f, 0.0f);
		glVertex3f(-1.57f, -0.85f, 0.0f);
		if (state[0] >= state[1])
			PlaySound(TEXT("D:\\left_1.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;
	}

	switch (state[1]) {

	case 3:

		//L
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.85f, 0.234f, 0.0f); // XYZ left, top
		glVertex3f(-1.76f, 0.234f, 0.0f); // XYZ right, top
		glVertex3f(-1.76f, -0.234f, 0.0f); // XYZ right, bottom
		glVertex3f(-1.85f, -0.234f, 0.0f); // XYZ left, bottom

		//M
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-1.73f, 0.18f, 0.0f); // XYZ left, top
		glVertex3f(-1.65, 0.18f, 0.0f); // XYZ right, top
		glVertex3f(-1.65f, -0.18f, 0.0f); // XYZ right, bottom
		glVertex3f(-1.73f, -0.18f, 0.0f); // XYZ left, bottom

		//S
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(-1.62f, 0.124f, 0.0f); // XYZ left, top
		glVertex3f(-1.54f, 0.124f, 0.0f); // XYZ right, top
		glVertex3f(-1.54f, -0.124f, 0.0f); // XYZ right, bottom
		glVertex3f(-1.62f, -0.124f, 0.0f); // XYZ left, bottom

		if (state[1] >= state[0] && state[1] >= state[2])
			PlaySound(TEXT("D:\\mid_3.wav"), NULL, SND_FILENAME | SND_LOOP);


		break;

	case 2:
		//L
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.85f, 0.234f, 0.0f); // XYZ left, top
		glVertex3f(-1.76f, 0.234f, 0.0f); // XYZ right, top
		glVertex3f(-1.76f, -0.234f, 0.0f); // XYZ right, bottom
		glVertex3f(-1.85f, -0.234f, 0.0f); // XYZ left, bottom

		//M
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-1.73f, 0.18f, 0.0f); // XYZ left, top
		glVertex3f(-1.65, 0.18f, 0.0f); // XYZ right, top
		glVertex3f(-1.65f, -0.18f, 0.0f); // XYZ right, bottom
		glVertex3f(-1.73f, -0.18f, 0.0f); // XYZ left, bottom

		if (state[1] >= state[0] && state[1] >= state[2])
			PlaySound(TEXT("D:\\mid_2.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;

	case 1:
		//L
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.85f, 0.234f, 0.0f); // XYZ left, top
		glVertex3f(-1.76f, 0.234f, 0.0f); // XYZ right, top
		glVertex3f(-1.76f, -0.234f, 0.0f); // XYZ right, bottom
		glVertex3f(-1.85f, -0.234f, 0.0f); // XYZ left, bottom
		
		if (state[1] >= state[0] && state[1] >= state[2])
			PlaySound(TEXT("D:\\mid_1.wav"), NULL, SND_FILENAME | SND_LOOP);

		break;
	}

	switch (state[2]) {

	case 3:
		// drawing a rectangle
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(-1.57f, 0.45f, 0.0f);
		glVertex3f(-1.50f, 0.41f, 0.0f);
		glVertex3f(-1.38f, 0.61f, 0.0f);
		glVertex3f(-1.44f, 0.65f, 0.0f);

		// drawing a rectangle
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-1.70f, 0.45f, 0.00f);
		glVertex3f(-1.63f, 0.41f, 0.0f);
		glVertex3f(-1.44f, 0.71f, 0.00f);
		glVertex3f(-1.50f, 0.75f, 0.0f);

		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.83f, 0.46f, 0.0f);
		glVertex3f(-1.75f, 0.41f, 0.0f);
		glVertex3f(-1.49f, 0.80f, 0.0f);
		glVertex3f(-1.57f, 0.85f, 0.0f);
		if (state[2] >= state[1])
			PlaySound(TEXT("D:\\right_3.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;

	case 2:
		// drawing a rectangle
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-1.70f, 0.45f, 0.00f);
		glVertex3f(-1.63f, 0.41f, 0.0f);
		glVertex3f(-1.44f, 0.71f, 0.00f);
		glVertex3f(-1.50f, 0.75f, 0.0f);

		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.83f, 0.46f, 0.0f);
		glVertex3f(-1.75f, 0.41f, 0.0f);
		glVertex3f(-1.49f, 0.80f, 0.0f);
		glVertex3f(-1.57f, 0.85f, 0.0f);
		if (state[2] >= state[1])
			PlaySound(TEXT("D:\\right_2.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;

	case 1:
		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(-1.83f, 0.46f, 0.0f);
		glVertex3f(-1.75f, 0.41f, 0.0f);
		glVertex3f(-1.49f, 0.80f, 0.0f);
		glVertex3f(-1.57f, 0.85f, 0.0f);
		if (state[2] >= state[1])
			PlaySound(TEXT("D:\\right_1.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;
	}

	switch (state[3]) {

	case 3:
		// drawing a rectangle
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(1.52f, 0.45f, 0.0f);
		glVertex3f(1.45f, 0.41f, 0.0f);
		glVertex3f(1.33f, 0.61f, 0.0f);
		glVertex3f(1.39f, 0.65f, 0.0f);

		// drawing a rectangle
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(1.65f, 0.45f, 0.00f);
		glVertex3f(1.58f, 0.41f, 0.0f);
		glVertex3f(1.39f, 0.71f, 0.00f);
		glVertex3f(1.45f, 0.75f, 0.0f);

		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.78f, 0.46f, 0.0f);
		glVertex3f(1.70f, 0.41f, 0.0f);
		glVertex3f(1.44f, 0.80f, 0.0f);
		glVertex3f(1.52f, 0.85f, 0.0f);
		if (state[3] >= state[4])
			PlaySound(TEXT("D:\\right_3.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;
	case 2:
		// drawing a rectangle
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(1.65f, 0.45f, 0.00f);
		glVertex3f(1.58f, 0.41f, 0.0f);
		glVertex3f(1.39f, 0.71f, 0.00f);
		glVertex3f(1.45f, 0.75f, 0.0f);

		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.78f, 0.46f, 0.0f);
		glVertex3f(1.70f, 0.41f, 0.0f);
		glVertex3f(1.44f, 0.80f, 0.0f);
		glVertex3f(1.52f, 0.85f, 0.0f);
		if (state[3] >= state[4])
			PlaySound(TEXT("D:\\right_2.wav"), NULL, SND_FILENAME | SND_LOOP);

		break;
	case 1:
		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.78f, 0.46f, 0.0f);
		glVertex3f(1.70f, 0.41f, 0.0f);
		glVertex3f(1.44f, 0.80f, 0.0f);
		glVertex3f(1.52f, 0.85f, 0.0f);
		if (state[3] >= state[4])
			PlaySound(TEXT("D:\\right_1.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;
	}

	switch (state[4]) {
	case 3:
		//L
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.80f, 0.234f, 0.0f); // XYZ left, top
		glVertex3f(1.71f, 0.234f, 0.0f); // XYZ right, top
		glVertex3f(1.71f, -0.234f, 0.0f); // XYZ right, bottom
		glVertex3f(1.80f, -0.234f, 0.0f); // XYZ left, bottom

		//M
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(1.68f, 0.18f, 0.0f); // XYZ left, top
		glVertex3f(1.60, 0.18f, 0.0f); // XYZ right, top
		glVertex3f(1.60f, -0.18f, 0.0f); // XYZ right, bottom
		glVertex3f(1.68f, -0.18f, 0.0f); // XYZ left, bottom

		//S
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(1.57f, 0.124f, 0.0f); // XYZ left, top
		glVertex3f(1.49f, 0.124f, 0.0f); // XYZ right, top
		glVertex3f(1.49f, -0.124f, 0.0f); // XYZ right, bottom
		glVertex3f(1.57f, -0.124f, 0.0f); // XYZ left, bottom
		if (state[4] >= state[3] && state[4] >= state[5])
			PlaySound(TEXT("D:\\mid_3.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;

	case 2:
		//L
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.80f, 0.234f, 0.0f); // XYZ left, top
		glVertex3f(1.71f, 0.234f, 0.0f); // XYZ right, top
		glVertex3f(1.71f, -0.234f, 0.0f); // XYZ right, bottom
		glVertex3f(1.80f, -0.234f, 0.0f); // XYZ left, bottom

		//M
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(1.68f, 0.18f, 0.0f); // XYZ left, top
		glVertex3f(1.60, 0.18f, 0.0f); // XYZ right, top
		glVertex3f(1.60f, -0.18f, 0.0f); // XYZ right, bottom
		glVertex3f(1.68f, -0.18f, 0.0f); // XYZ left, bottom
		if (state[4] >= state[3] && state[4] >= state[5])
			PlaySound(TEXT("D:\\mid_2.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;

	case 1:
		//L
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.80f, 0.234f, 0.0f); // XYZ left, top
		glVertex3f(1.71f, 0.234f, 0.0f); // XYZ right, top
		glVertex3f(1.71f, -0.234f, 0.0f); // XYZ right, bottom
		glVertex3f(1.80f, -0.234f, 0.0f); // XYZ left, bottom
		if (state[4] >= state[3] && state[4] >= state[5])
			PlaySound(TEXT("D:\\mid_1.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;
	}

	switch (state[5]) {
	case 3:
		// drawing a rectangle
		glColor3f(1.0, 0.0, 0.0);

		// drawing a rectangle
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(1.52f, -0.45f, 0.0f);
		glVertex3f(1.45f, -0.41f, 0.0f);
		glVertex3f(1.33f, -0.61f, 0.0f);
		glVertex3f(1.39f, -0.65f, 0.0f);

		// drawing a rectangle
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(1.65f, -0.45f, 0.00f);
		glVertex3f(1.58f, -0.41f, 0.0f);
		glVertex3f(1.39f, -0.71f, 0.00f);
		glVertex3f(1.45f, -0.75f, 0.0f);

		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.78f, -0.46f, 0.0f);
		glVertex3f(1.70f, -0.41f, 0.0f);
		glVertex3f(1.44f, -0.80f, 0.0f);
		glVertex3f(1.52f, -0.85f, 0.0f);
		if (state[5] >= state[4])
			PlaySound(TEXT("D:\\left_3.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;

	case 2:
		// drawing a rectangle
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(1.65f, -0.45f, 0.00f);
		glVertex3f(1.58f, -0.41f, 0.0f);
		glVertex3f(1.39f, -0.71f, 0.00f);
		glVertex3f(1.45f, -0.75f, 0.0f);

		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.78f, -0.46f, 0.0f);
		glVertex3f(1.70f, -0.41f, 0.0f);
		glVertex3f(1.44f, -0.80f, 0.0f);
		glVertex3f(1.52f, -0.85f, 0.0f);
		if (state[5] >= state[4])
			PlaySound(TEXT("D:\\left_2.wav"), NULL, SND_FILENAME | SND_LOOP);

		break;

	case 1:
		// drawing a rectangle
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.78f, -0.46f, 0.0f);
		glVertex3f(1.70f, -0.41f, 0.0f);
		glVertex3f(1.44f, -0.80f, 0.0f);
		glVertex3f(1.52f, -0.85f, 0.0f);
		if (state[5] >= state[4])
			PlaySound(TEXT("D:\\left_1.wav"), NULL, SND_FILENAME | SND_LOOP);
		break;
	}


	glEnd();
	glutSwapBuffers();
}

#pragma endregion




int main(int argc, char** argv) {


	/* 1) INITIALIZATION */
	// initialize GLUT
	glutInit(&argc, argv);
	// set window position and size
	glutInitWindowPosition(545, 180);
	glutInitWindowSize(720, 720);
	// set the combination of predefined values for display mode
	// set color space (Red, Green, Blue - RGB)
	// alocate depth buffer
	// set the size of the buffer (double)
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	// create window
	glutCreateWindow("Screen");
	/* 2) REGISTRATION OF CALLBACK FUNCTION */
	// function called when new window need to be drawn
	glutDisplayFunc(display);
	// function called when window changes the size
	glutReshapeFunc(reshape);
	// function called when nothing else is executing and CPU is free
	glutIdleFunc(idle);
	// function called when keyboard key is pressed
	//glutKeyboardFunc(readSensors); // custom function 'readSensors' can	be implemented separately
	loadTexture();
	initGL();
	/* 3) START GLUT PROCESSING CYCLE */
	glutMainLoop();


	return 0;
}




