/*	This is a C++ version of Pong created using the tutorial at "https://noobtuts.com/cpp/2d-pong-game"
	"Written" by Zachary Hersant. It is much the same as the tutorial, with slight differences due to experimentation
	and solving problems introduced by newer methods. */

#include "pch.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <sstream> 
#include <math.h> 
#include <gl\gl.h>
#include <gl\glu.h>
#include "GL/freeglut.h"
#pragma comment(lib, "OpenGL32.lib")

// keycodes
#define VK_W 0x57
#define VK_S 0x53

void draw();
void update(int value);
void drawText(float x, float y, std::string text);
void enable2D(int width, int height);
void drawRect(float x, float y, float width, float height);
void keyboard();
void updateBall();
void vec2_norm(float& x, float &y);

//window size and fps
int width = 500;
int height = 200;
int interval = 1000 / 60;

//score
int score_left = 0;
int score_right = 0;

//racketstats
int racket_width = 10;
int racket_height = 80;
int racket_speed = 3;
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;
float racket_right_x = width - racket_width - 10.0;
float racket_right_y = 50.0f;

//ball stats
float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 8;
float ball_speed = 2.0;

//main program
int main(int argc, char** argv)
{
    //initialize openGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Pong");

	// register callback functions
	glutDisplayFunc(draw);
	glutTimerFunc(interval, update, 0);

	// setup scene to 2d mode and set draw color to white
	enable2D(width, height);
	glColor3f(1.0f, 1.0f, 1.0f);

	//start program
	glutMainLoop();
	return 0;
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// To Do: draw out scene
	//draw rackets
	drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
	drawRect(racket_right_x, racket_right_y, racket_width, racket_height);

	//draw ball
	drawRect(ball_pos_x - ball_size / 2, ball_pos_y - ball_size / 2, ball_size, ball_size);

	//draw score
	drawText(width / 2 - 10, height - 20, std::to_string(score_left) + ":" + std::to_string(score_right));

	// swap buffers (has to be done at the end)
	glutSwapBuffers();
}

void update(int value) {
	//input handling
	keyboard();

	// call update() again in 'interval' milliseconds
	glutTimerFunc(interval, update, 0);

	//update ball
	updateBall();

	// redisplay frame
	glutPostRedisplay();
}

void enable2D(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void drawText(float x, float y, std::string text) {
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());

}

void drawRect(float x, float y, float width, float height) {
	//draws a rectangle, each glVertex2f function describes a corner of the rectangle
	glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
	glEnd();
}

void keyboard() {
	//left racket
	if (GetAsyncKeyState(VK_W)) racket_left_y += racket_speed;
	if (GetAsyncKeyState(VK_S)) racket_left_y -= racket_speed;

	//right racket
	if (GetAsyncKeyState(VK_UP)) racket_right_y += racket_speed;
	if (GetAsyncKeyState(VK_DOWN)) racket_right_y -= racket_speed;
}

void updateBall() {
	//fly a bit
	ball_pos_x += ball_dir_x * ball_speed;
	ball_pos_y += ball_dir_y * ball_speed;

	//hit by left racket
	if (ball_pos_x < racket_left_x + racket_width &&
		ball_pos_x > racket_left_x &&
		ball_pos_y < racket_left_y + racket_height &&
		ball_pos_y > racket_left_y) {
		float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
		ball_dir_x = fabs(ball_dir_x);
		ball_dir_y = t;
		ball_speed += 0.2;
	}

	//hit by right racket
	if (ball_pos_x > racket_right_x &&
		ball_pos_x < racket_right_x + racket_width &&
		ball_pos_y < racket_right_y + racket_height &&
		ball_pos_y > racket_right_y) {
		// set fly direction depending on where it hit the racket
		float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
		ball_dir_x = -fabs(ball_dir_x);
		ball_dir_y = t;
		ball_speed += 0.2;
	}

	//hit left wall
	if (ball_pos_x < 0) {
		++score_right;
		ball_pos_x = width / 2;
		ball_pos_y = height / 2;
		ball_dir_x = fabs(ball_dir_x);
		ball_dir_y = 0;
	}

	//hit right wall
	if (ball_pos_x < 0) {
		++score_left;
		ball_pos_x = width / 2;
		ball_pos_y = height / 2;
		ball_dir_x = -fabs(ball_dir_x);
		ball_dir_y = 0;
	}


	//hit top wall
	if (ball_pos_y > height) {
		ball_dir_y = -fabs(ball_dir_y); //flips vertical dir
	}

	//hit bottom wall
	if (ball_pos_y < 0) {
		ball_dir_y = fabs(ball_dir_y); //flips vertical dir
	}

	//make sure length of dir stays at 1
	vec2_norm(ball_dir_x, ball_dir_y);
}

void vec2_norm(float& x, float &y) {
	// sets a vectors length to 1 (which means that x + y == 1)
	float length = sqrt((x * x) + (y * y));
	if (length != 0.0f) {
		length = 1.0f / length;
		x *= length;
		y *= length;
	}
}