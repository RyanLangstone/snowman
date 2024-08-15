/******************************************************************************
 *
 * Animation v1.0 (23/02/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene.
 *
 ******************************************************************************/
#define _USE_MATH_DEFINES

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;


const unsigned int FramePixels = 700;
/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_F			102 // q key.
#define KEY_Q			113 // q key.
#define KEY_S			115 // s key.

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);
void mouse(int button, int state, int x, int y);
void printText(char text[], float x, float y);
void circle(float radius, float x, float y, float centerX, float centerY, float centerColor[4], float outerColor[4], float startPoint, float endPoint, bool background);
void rotate(float angle, float xInitial, float yInitial, float* xFinal, float* yFinal);
void alterLanscape(float x, float y);
/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char** argv);
void init(void);
void think(void);
void birdfunc();
/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

typedef struct {
	float x;
	float y;

}Position2;

typedef struct {
	// y=a(x-x2)^2 +y2 or // y=b(x-x2)^2 +y2
	float A;
	float B;
	float X2;
	float Y2;
}Quadric;

typedef struct {
	Position2 location;
	float size;
	float dy;
	int landTime;
	int lifetime;
	int depth;
	float transparancy;
	bool active;
}Partical;

typedef struct {
	Position2 location;
	Quadric formula;
	float dx;
	float theta;
}bird;

typedef struct {
	float x2 ;
	float y2;
	float A;
	float B ;
	float x3;
	float y3;
	float A2;
	float B2;
	int state;
}flame;
flame fireEquation;

Partical  snow[50001];
float lanscape[200];
float snowHeight[4][200];

bird birds[31];
int activeBird[31];
int totalActiveBirds = 0;
float angle = M_PI / 4;

int framesPassed = 1;
int totalSnow = 50;
bool snowfall = true;
GLfloat clickpos[2] = { 0,0 };

float lightningPoints[5];
float lightningSpawn = -1000;
bool fire = true;
/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char** argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(FramePixels, FramePixels);
	glutInitWindowPosition(2150, 100); // so it displays on monitor, coment out when not uning monitor
	glutCreateWindow("Animation");


	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUAD_STRIP);
	int num = 0;
	for (float i = -1; i < 1; i += 0.0101) {
		glColor4f(0.15294, 0.56863, 0.0667, 1);
		glVertex2f(i, -1);
		glColor4f(0.298, 0.6902, 0.0196, 1);
		glVertex2f(i, lanscape[num]);
		num++;
	}
	glEnd();
	float circleCenter[4] = { 0.16863,0.117647,0.02353,1 };
	float circleOuter[4] = { 0.25098, 0.17255, 0.02353,1 };
	circle(0.2, 0.53, lanscape[145], 0.53, lanscape[145], circleCenter, circleOuter, 0, 2 * M_PI, false);


	glBegin(GL_QUAD_STRIP); // sky to be invert of the ground
	
	num = 0;
	for (float i = -1; i <= 1; i += 0.0101) {
		glColor4f(0.557, 0.808, 1, 0.9);
		glVertex2f(i, 1);
		glColor4f(0.647, 0.898, 0.9686274, 1);
		glVertex2f(i, lanscape[num]);
		num++;
	}
	glEnd();
	bool firstPass; // variable so that you can run some things only once to improve eficency for example background calculation in circle function
	if (framesPassed == 1) { firstPass = true; }
	else { firstPass = false; }

	
	for (int i = 0; i < 5000; i++) {
		if (snow[i].depth <= 1 && snow[i].active) {
			glColor4f(1, 1, 1, snow[i].transparancy);
			glPointSize(snow[i].size);
			glBegin(GL_POINTS);
			glVertex2f(snow[i].location.x, snow[i].location.y);
			glEnd();
		}
	}
	// stick arms coming out of snowman
	glLineWidth(4);
	glColor4f(0.388235, 0.2, 0.0235, 1);
	glBegin(GL_LINES);
	glVertex2f(-0.61 , lanscape[50] + 0.345);
	glVertex2f(-0.7 , lanscape[50] + 0.235);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(-0.39, lanscape[50] + 0.345);
	glVertex2f(-0.3, lanscape[50] + 0.235);
	glEnd();
	// main 3 circles for snowman
	float snowmanCenterColor[4] = { 1,1,1,1 };
	float snowmanCuterColor[4] = { 0.6902, 0.83137, 0.8196,1 };
	circle(0.15, -0.5, lanscape[50] + 0.1, -0.5, lanscape[50] + 0.1, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.12, -0.5, lanscape[50] + 0.345, -0.5, lanscape[50] + 0.345, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.07, -0.5, lanscape[50] + 0.525, -0.5, lanscape[50] + 0.525, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	glColor4f(0.388235, 0.2, 0.0235, 1);
	// the rest of the stick arms, drawn on top of the 3 balls
	glBegin(GL_LINES);
		glVertex2f(-0.7, lanscape[50] + 0.235);
		glVertex2f(-0.5 + 0.15*sin(290 *M_PI / 180), lanscape[50] + 0.1 + 0.15*cos(290 * M_PI / 180));
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(-0.5 + 0.15 * sin(290 * M_PI / 180), lanscape[50] + 0.1 + 0.15 * cos(290 * M_PI / 180));
		glVertex2f(-0.64, lanscape[50] + 0.08);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(-0.5 + 0.15 * sin(290 * M_PI / 180), lanscape[50] + 0.1 + 0.15 * cos(290 * M_PI / 180));
		glVertex2f(-0.6, lanscape[50] + 0.1);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(-0.5 + 0.15 * sin(290 * M_PI / 180), lanscape[50] + 0.1 + 0.15 * cos(290 * M_PI / 180));
		glVertex2f(-0.6, lanscape[50] + 0.16);
	glEnd();

	glBegin(GL_LINES);
		glVertex2f(-0.3, lanscape[50] + 0.235);
		glVertex2f(-0.5 + 0.15 * sin(75 * M_PI / 180), lanscape[50] + 0.1 + 0.15 * cos(75 * M_PI / 180));
	glEnd();
	glBegin(GL_LINES);
		glVertex2f(-0.5 + 0.15 * sin(75 * M_PI / 180), lanscape[50] + 0.1 + 0.15 * cos(75 * M_PI / 180));
		glVertex2f(-0.36, lanscape[50] + 0.08);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(-0.5 + 0.15 * sin(75 * M_PI / 180), lanscape[50] + 0.1 + 0.15 * cos(75 * M_PI / 180));
	glVertex2f(-0.4, lanscape[50] + 0.1);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(-0.5 + 0.15 * sin(75 * M_PI / 180), lanscape[50] + 0.1 + 0.15 * cos(75 * M_PI / 180));
	glVertex2f(-0.4, lanscape[50] + 0.16);
	glEnd();
	// added buttons, eyes and mouth features
	snowmanCenterColor[0] = 0; snowmanCenterColor[1] = 0; snowmanCenterColor[2] = 0;
	snowmanCuterColor[0] = 0; snowmanCuterColor[1] = 0; snowmanCuterColor[2] = 0;
	circle(0.013, -0.47, lanscape[50] + 0.555, -0.47, lanscape[50] + 0.555, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.013, -0.53, lanscape[50] + 0.555, -0.53, lanscape[50] + 0.555, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);

	circle(0.008, -0.46, lanscape[50] + 0.5, -0.46, lanscape[50] + 0.5, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.008, -0.54, lanscape[50] + 0.5, -0.54, lanscape[50] + 0.5, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.008, -0.48, lanscape[50] + 0.495, -0.48, lanscape[50] + 0.495, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.008, -0.52, lanscape[50] + 0.495, -0.52, lanscape[50] + 0.495, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.008, -0.5, lanscape[50] + 0.49, -0.5, lanscape[50] + 0.49, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);

	circle(0.013, -0.5, lanscape[50] + 0.385, -0.5, lanscape[50] + 0.385, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.013, -0.5, lanscape[50] + 0.305, -0.5, lanscape[50] + 0.305, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.013, -0.5, lanscape[50] + 0.05, -0.5, lanscape[50] + 0.05, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.013, -0.5, lanscape[50] + 0.15, -0.5, lanscape[50] + 0.15, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	// nose
	snowmanCenterColor[0] = 0.92157; snowmanCenterColor[1] = 0.53725; snowmanCenterColor[2] = 0.129412;
	circle(0.01, -0.5, lanscape[50] + 0.5225, -0.5, lanscape[50] + 0.5225, snowmanCenterColor, snowmanCenterColor, 1 * M_PI, 2 * M_PI, firstPass);
	glBegin(GL_POLYGON);
		glVertex2f(-0.5, lanscape[50] + 0.51);
		glVertex2f(-0.5, lanscape[50] + 0.535);
		glVertex2f(-0.47, lanscape[50] + 0.545);
	glEnd();
	// scarf alternating red and white
	float scarfColor[4] = {1,0,0,1};
	glLineWidth(1.5); 
	for (float i = 0.355; i < 0.41; i += 0.008) {
		if (scarfColor[1] == 0) { scarfColor[1] = 1; scarfColor[2] = 1; }
		else { scarfColor[1] = 0; scarfColor[2] = 0; }
		glColor4f(scarfColor[0], scarfColor[1], scarfColor[2], scarfColor[3]);

		glBegin(GL_LINES); 
		glVertex2f(-0.2875 + i * sin(1.6 * M_PI), lanscape[50] + 0.195 + i * cos(1.6 * M_PI)); 
		glVertex2f(-0.2875 + i * sin(1.6 * M_PI), lanscape[50] + 0.15 + i * cos(1.6 * M_PI)); 
		alterLanscape(-0.2875 + i * sin(1.6 * M_PI), lanscape[50] + 0.195 + i * cos(1.6 * M_PI));
		glEnd(); 
	}

	glBegin(GL_QUAD_STRIP);
	for (float i = 1.6 * M_PI; i <= 1.75 * M_PI; i += 0.01) {
		if ((int)(i * 102) % 4 == 0) {
			if (scarfColor[1] == 0) { scarfColor[1] = 1; scarfColor[2] = 1; }
			else { scarfColor[1] = 0; scarfColor[2] = 0; }
		}
		glColor4f(scarfColor[0], scarfColor[1], scarfColor[2], scarfColor[3]);
		glVertex2f(-0.285 + 0.355 * sin(i), lanscape[50] + 0.185 + 0.355 * cos(i));
		glVertex2f(-0.285 + 0.41 * sin(i), lanscape[50] + 0.185 + 0.41 * cos(i));
		alterLanscape(-0.285 + 0.41 * sin(i), lanscape[50] + 0.185 + 0.41 * cos(i));
	}
	glEnd();
	
	glBegin(GL_QUAD_STRIP);
	for (float i = 0.8*M_PI; i <= 1.2*M_PI; i += 0.01) {
		if ((int)(i * 100) % 10 == 0) {
			if (scarfColor[1] == 0) {scarfColor[1] = 1; scarfColor[2] = 1;}
			else { scarfColor[1] = 0; scarfColor[2] = 0; }
		}
		glColor4f(scarfColor[0], scarfColor[1], scarfColor[2], scarfColor[3]);
		glVertex2f(-0.5 + 0.115 * sin(i), lanscape[50] + 0.585 + 0.115 * cos(i));
		glVertex2f(-0.5 + 0.17 * sin(i), lanscape[50] + 0.585 + 0.17 * cos(i));
	
	}
	glEnd();

	
	// makes snow of depth level 2 render infront of objects
	for (int i = 0; i < 5000; i++) {
		if (snow[i].depth == 2 && snow[i].active) {
			glColor4f(1, 1, 1, snow[i].transparancy);
			glPointSize(snow[i].size);
			glBegin(GL_POINTS);
			glVertex2f(snow[i].location.x, snow[i].location.y);
			glEnd();
		}
	}



	float logCenterColor[4] = { 0.6392, 0.3412, 0.0627,1 };
	float logOuterColor[4] = { 0.388235, 0.2, 0.0235,1 };
	circle(0.05, 0.5, lanscape[145] + 0.78 -0.785, 0.46, lanscape[145] + 0.78 -0.765, logCenterColor, logOuterColor, 1.5 * M_PI, 1.815 * M_PI, false);
	glBegin(GL_QUAD_STRIP);
		glColor4f(0.388235, 0.2, 0.0235,1);
		glVertex2f(0.63, lanscape[145] + 0.78 -0.825); //lanscape[145]+0.79
		glVertex2f(0.45, lanscape[145] + 0.78 -0.785); //lanscape[150]-0.09
		glColor4f(0.6392, 0.3412, 0.0627,1);
		glVertex2f(0.64, lanscape[145] + 0.78 -0.80); //lanscape[150]-0.05
		glVertex2f(0.46, lanscape[145] + 0.78 -0.765);
		glColor4f(0.388235, 0.2, 0.0235,1);
		glVertex2f(0.65, lanscape[145] + 0.78 -0.775);
		glVertex2f(0.47, lanscape[145] + 0.78 -0.745);
	glEnd();
	circle(0.023, 0.558, lanscape[145] + 0.78 -0.716, 0.56, lanscape[145] + 0.78 -0.705, logCenterColor, logOuterColor, 1.7 * M_PI, 2.45 * M_PI, false);
	glBegin(GL_QUAD_STRIP);
		glColor4f(0.388235, 0.2, 0.0235,1);
		glVertex2f(0.43, lanscape[145] + 0.78 -0.845);
		glVertex2f(0.54, lanscape[145] + 0.78 -0.7);
		glColor4f(0.6392, 0.3412, 0.0627,1);
		glVertex2f(0.455, lanscape[145] + 0.78 -0.8525);
		glVertex2f(0.56, lanscape[145] + 0.78 -0.705);
		glColor4f(0.388235, 0.2, 0.0235,1);
		glVertex2f(0.48, lanscape[145] + 0.78 -0.86);
		glVertex2f(0.58, lanscape[145] + 0.78 -0.71);
	glEnd();
	circle(0.0235, 0.498, lanscape[145] + 0.78 -0.713, 0.49, lanscape[145] + 0.78 -0.7, logCenterColor, logOuterColor, 1.5 * M_PI, 2.1 * M_PI, false);
	glBegin(GL_QUAD_STRIP);
		glColor4f(0.388235, 0.2, 0.0235,1);
		glVertex2f(0.6, lanscape[145] + 0.78 -0.87);
		glVertex2f(0.475, lanscape[145] + 0.78 -0.71);
		glColor4f(0.6392, 0.3412, 0.0627,1);
		glVertex2f(0.6225, lanscape[145] + 0.78 -0.86);
		glVertex2f(0.49, lanscape[145] + 0.78 -0.7);
		glColor4f(0.388235, 0.2, 0.0235,1);
		glVertex2f(0.645, lanscape[145] + 0.78 -0.85);
		glVertex2f(0.505, lanscape[145] + 0.78 -0.69);
	glEnd();

	
	if (fire == true && lightningSpawn + 6 < framesPassed) {
		circleCenter[0] = 1;
		circleCenter[1] = 0;
		circleCenter[2] = 0;
		glColor4f(1, 0, 0, 1);
		//circle(0.09, 0.53, lanscape[145], 0.53, lanscape[145], circleCenter, circleCenter, 0, 2* M_PI, false);
		//x=A(y-y2)^2 + x2		vs		y= a(x-x2) + y2

		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1, 0, 0, 1);
		glVertex2f(0.53, lanscape[145] + 0.07);
		glColor4f(1, 0.6196, 0, 0.8);
		for (float y = lanscape[145] - 0.08; y < fireEquation.y2; y += 0.001) {glVertex2f((float)(fireEquation.A * pow((y - fireEquation.y2), 2) + fireEquation.x2), y);}
		for (float y = fireEquation.y2; y <= lanscape[145] + 0.15; y += 0.001) { glVertex2f(fireEquation.B * pow((y - fireEquation.y2), 2) + fireEquation.x2, y); }
		for (float y = lanscape[145] - 0.08; y < fireEquation.y3; y += 0.001) {glVertex2f(fireEquation.A2 * pow((y - fireEquation.y3), 2) + fireEquation.x3, y);}
		for (float y = fireEquation.y3; y <= lanscape[145] + 0.15; y += 0.001) {glVertex2f(fireEquation.B2 * pow((y - fireEquation.y3), 2) + fireEquation.x3, y);}
		glEnd();
	}
	circle(0.0235, 0.522, lanscape[145] + 0.78 -0.72, 0.5175, lanscape[145] + 0.78 -0.705, logCenterColor, logOuterColor, 1.60 * M_PI, 2.2 * M_PI, false);
	glBegin(GL_QUAD_STRIP);
		glColor4f(0.388235, 0.2, 0.0235,1);
		glVertex2f(0.51, lanscape[145] + 0.78 -0.89);
		glVertex2f(0.50, lanscape[145] + 0.78 -0.71);
		glColor4f(0.6392, 0.3412, 0.0627,1);
		glVertex2f(0.535, lanscape[145] + 0.78 -0.885);
		glVertex2f(0.5175, lanscape[145] + 0.78 -0.705);
		glColor4f(0.388235, 0.2, 0.0235,1);
		glVertex2f(0.56, lanscape[145] + 0.78 -0.88);
		glVertex2f(0.535, lanscape[145] + 0.78 -0.7);
	glEnd();


	float stoneCenterColor[4] = { 0.77255, 0.77255, 0.77255,1 };
	float stoneOuterColor[4] = { 0.67843, 0.65098, 0.62745,1 };
	circle(0.02, 0.53 + 0.15 * sin(1.5 * M_PI), lanscape[145] - 0.04 + 0.115* cos(1.5 * M_PI), 0.53 + 0.15 * sin(1.5 * M_PI), lanscape[145] - 0.04 + 0.115* cos(1.5* M_PI), stoneCenterColor, stoneOuterColor, 0, 2 * M_PI, false);
	circle(0.02, 0.53 + 0.15 * sin(1 * M_PI), lanscape[145] - 0.04 + 0.115* cos(1 * M_PI), 0.53 + 0.15 * sin(1 * M_PI), lanscape[145] - 0.04 + 0.115* cos(1 * M_PI), stoneCenterColor, stoneOuterColor, 0, 2 * M_PI, false);
	circle(0.02, 0.53 + 0.15 * sin(0.5 * M_PI), lanscape[145] - 0.04 + 0.115* cos(0.5 * M_PI), 0.53 + 0.15 * sin(0.5 * M_PI), lanscape[145] - 0.04 + 0.115* cos(0.5 * M_PI), stoneCenterColor, stoneOuterColor, 0, 2 * M_PI, false);
	circle(0.02, 0.53 + 0.15 * sin(0.75 * M_PI), lanscape[145] - 0.04 + 0.115* cos(0.75 * M_PI), 0.53 + 0.15 * sin(0.75 * M_PI), lanscape[145] - 0.04 + 0.115* cos(0.75 * M_PI), stoneCenterColor, stoneOuterColor, 0, 2 * M_PI, false);
	circle(0.02, 0.53 + 0.15 * sin(1.25 * M_PI), lanscape[145] - 0.04 + 0.115* cos(1.25 * M_PI), 0.53 + 0.15 * sin(1.25 * M_PI), lanscape[145] - 0.04 + 0.115* cos(1.25 * M_PI), stoneCenterColor, stoneOuterColor, 0, 2 * M_PI, false);


	char text[] = "bird";
	for (int i = 0; i < 4; i++) {
		glRasterPos2f(-0.5, 0.8);

		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, "h");
	}

	for (int i = 0; i < 80; i++) {
		if (activeBird[i] == 1) {
			glLoadIdentity();
			glPushMatrix();
			glTranslatef(birds[i].location.x, birds[i].location.y, 0);

			glRotatef(birds[i].theta, 0.0, 0.0, 1.0);
			//glTranslatef(-birds[i].location.x, -birds[i].location.y, 0);
			float x, y;
			float birdCenterColor[4] = { 0.2118, 0.1529, 0,1 };
			float birdOuterColor[4] = { 0.2118, 0.1529, 0,1 };
			glColor4f(0.2118, 0.1529, 0,1);

			

			glBegin(GL_POLYGON);
			rotate(0, 0.035355, 0.017678, &x, &y);
			glVertex2f(0 + x, 0 + y);
			glVertex2f(0 - x, 0 + y);
			glVertex2f(0 - x, 0 - y);
			glVertex2f(0 + x, 0 - y);
			glEnd();

			glBegin(GL_POLYGON);
			rotate(0, 0.008445, 0.035606602, &x, &y);
			glVertex2f(0 + x, 0 + y);
			rotate(0, -0.030445, 0.035606602, &x, &y);
			glVertex2f(0 + x, 0 + y);
			rotate(0, -0.030445, 0.014393398, &x, &y);
			glVertex2f(0 + x, 0 + y);
			rotate(0, 0.008445, 0.014393398, &x, &y);
			glVertex2f(0 + x, 0 + y);
			glEnd();

			glBegin(GL_TRIANGLES);
			glVertex2f(0 + 0.005, 0 + 0.045);
			glVertex2f(0 + 0.005, 0 + 0.015);
			glVertex2f(0 + 0.02, 0 + 0.015);
			glEnd();

			glBegin(GL_TRIANGLES);
			glVertex2f(0 - 0.035, 0 + 0.03);
			glVertex2f(0 + 0.01, 0 + 0.03);
			glVertex2f(0 + 0.01, 0 + 0.063);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex2f(0 + 0.035, 0 + 0.01);
			glVertex2f(0 + 0.06, 0 + 0.01);
			glVertex2f(0 + 0.06, 0 + 0.02);
			glVertex2f(0 + 0.035, 0 + 0.02);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex2f(0 - 0.05, 0 + 0.0025);
			glVertex2f(0 - 0.0625, 0 + 0.0025);
			glVertex2f(0 - 0.0625, 0 + 0.015);
			glVertex2f(0 + 0.05, 0 + 0.015);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex2f(0 - 0.05, 0 + 0.02);
			glVertex2f(0 - 0.06, 0 + 0.02);
			glVertex2f(0 - 0.06, 0 + 0.015);
			glVertex2f(0 + 0.05, 0 + 0.015);
			glEnd();

			glBegin(GL_TRIANGLES);
			glVertex2f(0 - 0.045, 0 + 0.02);
			glVertex2f(0 - 0.065, 0 + 0.0175);
			glVertex2f(0 - 0.065, 0 + 0.0275);
			glEnd();


			glColor4f(0, 0, 0,1);
			glBegin(GL_TRIANGLES);
			glVertex2f(0 + 0.06, 0 + 0.01);
			glVertex2f(0 + 0.06, 0 + 0.02);
			glVertex2f(0 + 0.07, 0 + 0.012);
			glEnd();
			glColor4f(0.2118, 0.1529, 0,1);

			circle(0.005, 0 - 0.0625, 0 + 0.007375, 0 - 0.0625, 0 + 0.007375, birdCenterColor, birdOuterColor, 1 * M_PI, 2 * M_PI, false);
			circle(0.00375, 0 - 0.065, 0 + 0.02375, 0 - 0.065, 0 + 0.02375, birdCenterColor, birdOuterColor, 1 * M_PI, 2 * M_PI, false);
			circle(0.0275, 0 - 0.005, 0 + (0.035 / sin(M_PI / 4)) * sin(angle), 0 + (0.005 / sin(M_PI / 4)) * cos(angle), 0 + (0.03 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1.25 * M_PI, 2.15 * M_PI, false);
			circle(0.03, 0 - 0.035, 0 + 0.05, 0 - 0.05, 0 + 0.01, birdCenterColor, birdOuterColor, 1 * M_PI, 1.17 * M_PI, false);
			circle(0.3, 0 + (0.015 / sin(M_PI / 4)) * cos(angle), 0 + (0.277 / sin(M_PI / 4)) * sin(angle), 0 + (0.015 / sin(M_PI / 4)) * cos(angle), 0 + (-0.015 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1 * M_PI - M_PI / 4 + angle, 1.055 * M_PI - M_PI / 4 + angle, false);
			circle(0.052, 0 - (0.00 / sin(M_PI / 4)) * cos(angle), 0 + (0.022 / sin(M_PI / 4)) * sin(angle), 0 - (0.03 / sin(M_PI / 4)) * cos(angle), 0 + (0.018 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1.2 * M_PI - M_PI / 4 + angle, 1.46 * M_PI - M_PI / 4 + angle, false);
			circle(0.02, 0 + 0.056569 * cos(angle), 0 + (0.018 / sin(M_PI / 4)) * sin(angle), 0 + (0.04 / sin(M_PI / 4)) * cos(angle), 0 + (0.015 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1.45 * M_PI, 2.55 * M_PI, false);
			circle(0.055, 0 + 0.120202 * cos(angle), 0 - (0.04 / sin(M_PI / 4)) * sin(angle), 0 + (0.035 / sin(M_PI / 4)) * cos(angle), 0 + (0.01 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1.625 * M_PI - M_PI / 4 + angle, 1.9 * M_PI - M_PI / 4 + angle, false);
			circle(0.02, 0 + (0.015 / sin(M_PI / 4)) * cos(angle), 0 - (0.003 / sin(M_PI / 4)) * sin(angle), 0 + (0.015 / sin(M_PI / 4)) * cos(angle), 0 - (0.015 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, M_PI - M_PI / 4, 1 * M_PI - M_PI / 4 + angle, false);
			float eyeColour[4] = { 1,1,1,1 };
			circle(0.004, 0 + 0.045, 0 + 0.025, 0 + 0.045, 0 + 0.025, eyeColour, eyeColour, 0 * M_PI, 2 * M_PI, false);

			glPopMatrix();
		}
	}
	glColor4f(0, 0, 0,1);
	printText("Number of Snow Particles:", -0.95, 0.85); // prints the snow amount
	for (int i = 0; i < 4; i++) {
		glRasterPos2f(-0.95 + 0.0225 * (strlen("Number of Snow Particles:") + i), 0.85);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (totalSnow / (int)pow(10, 3 - i)) % 10 + 48);
	}
	printText("/5000", -0.95 + (strlen("Number of Snow Particles:4000")) * 0.0225, 0.85);

	printText("Number of Birds:", -0.95, 0.8);// prints the bird amount
	if (totalActiveBirds < 10) {
		glRasterPos2f(-0.95 + 0.0225 * strlen("Number of Birds:"), 0.8);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, totalActiveBirds + 48);
	}
	else {
		for (int i = 0; i < 2; i++) {
			glRasterPos2f(-0.95 + 0.0225 * (strlen("Number of Birds:") + i), 0.8);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (totalActiveBirds / (int)pow(10, 1 - i)) % 10 + 48);
		}
	}
	printText("Press q to exit", -0.95, 0.75);// prints the comands
	printText("Press s to stop snow", -0.95, 0.7);// prints the comands
	printText("Press f to toggle fire", -0.95, 0.65);// prints the comands
	printText("Click to summon bird", -0.95, 0.6);// prints the comands
	glColor4f(0.95, 0.95, 0.95,1);
	if (fire == true) {
		if (lightningSpawn+2 > framesPassed) {
			glLineWidth(7);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[0],1);
				glVertex2f(lightningPoints[1], 0.55);
			glEnd();
		}else if (lightningSpawn + 4 > framesPassed) {
			glLineWidth(7);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[0], 1);
				glVertex2f(lightningPoints[1], 0.55);
			glEnd();
			glLineWidth(6);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[1], 0.55);
				glVertex2f(lightningPoints[2], 0.15);
			glEnd();
		}
		else if (lightningSpawn + 6 > framesPassed) {
			glLineWidth(7);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[0], 1);
				glVertex2f(lightningPoints[1], 0.55);
			glEnd();
			glLineWidth(6);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[1], 0.55);
				glVertex2f(lightningPoints[2], 0.15);
			glEnd();
			glLineWidth(5);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[2], 0.15);
				glVertex2f(lightningPoints[3], -0.25);
			glEnd();
		}
		else if (lightningSpawn + 40 > framesPassed) {
			glLineWidth(7);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[0], 1);
				glVertex2f(lightningPoints[1], 0.55);
			glEnd();
			glLineWidth(6);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[1], 0.55);
				glVertex2f(lightningPoints[2], 0.15);
			glEnd();
			glLineWidth(5);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[2], 0.15);
				glVertex2f(lightningPoints[3], -0.25);
			glEnd();
			glLineWidth(4);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[3], -0.25);
				glVertex2f(lightningPoints[4], lanscape[145] + 0.05);
			glEnd();
		}
		else if (lightningSpawn + 42> framesPassed) {
			glLineWidth(6);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[1], 0.55);
				glVertex2f(lightningPoints[2], 0.15);
			glEnd();
			glLineWidth(5);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[2], 0.15);
				glVertex2f(lightningPoints[3], -0.25);
			glEnd();
			glLineWidth(4);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[3], -0.25);
				glVertex2f(lightningPoints[4], lanscape[145] + 0.05);
			glEnd();
		}
		else if (lightningSpawn + 44 > framesPassed) {
			glLineWidth(5);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[2], 0.15);
				glVertex2f(lightningPoints[3], -0.25);
			glEnd();
			glLineWidth(4);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[3], -0.25);
				glVertex2f(lightningPoints[4], lanscape[145] + 0.05);
			glEnd();
		}
		else if (lightningSpawn + 46 > framesPassed) {
			glLineWidth(4);
			glBegin(GL_LINES);
				glVertex2f(lightningPoints[3], -0.25);
				glVertex2f(lightningPoints[4], lanscape[145] + 0.05);
			glEnd();
		}
		
	}
	
	

	glutSwapBuffers();
}
void printText(char text[], float x, float y) {
	for (int i = 0; i < strlen(text); i++) {
		glRasterPos2f(x + 0.0225 * i, y);
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
	}
}

void rotate(float angle, float xInitial, float yInitial, float* xFinal, float* yFinal) {
	*xFinal = xInitial * cos(angle) - yInitial * sin(angle);
	*yFinal = xInitial * sin(angle) + yInitial * cos(angle);
}

void circle(float radius, float x, float y, float centerX, float centerY, float centerColor[4], float outerColor[4], float startPoint, float endPoint, bool background) {
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(centerColor[0], centerColor[1], centerColor[2], centerColor[3]);
	glVertex2f(centerX, centerY);
	glColor4f(outerColor[0], outerColor[1], outerColor[2], outerColor[3]);
	if (background == false) {
		for (float i = startPoint; i <= endPoint; i += 0.01) {
			glVertex2f(x + radius * sin(i), y + radius * cos(i));
		}

	}
	else {
		for (float i = startPoint; i <= endPoint; i += 0.01) {
			glVertex2f(x + radius * sin(i), y + radius * cos(i));
			alterLanscape(x + radius * sin(i), y + radius * cos(i));
		}
	}
	glEnd();
}
void alterLanscape(float x, float y) {
	int heightIndex = round((x +1)* 100);
	if (snowHeight[1][heightIndex] < y) {
		snowHeight[1][heightIndex] = y - 0.003;
	}
	if (snowHeight[2][heightIndex] > y) {
		snowHeight[2][heightIndex] = y - 0.003;
		snowHeight[3][heightIndex] = y - 0.003;
	}
 }

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		clickpos[0] = ((GLfloat)x / (GLfloat)FramePixels) * 2 - 1;
		clickpos[1] = ((GLfloat)y / (GLfloat)FramePixels) * -2 + 1;
		birdfunc();
	}
}
void birdfunc(void) {
	float p1y = (((float)rand() / RAND_MAX) * 0.5f);
	p1y = p1y * (clickpos[0] + 1);
	if (rand() % 2 == 1) { p1y = -p1y; }
	p1y += clickpos[1];
	float p2y = (((float)rand() / RAND_MAX) * 0.5f);
	p2y = p2y * ((clickpos[0] - 1) * -1);
	if (rand() % 2 == 1) { p2y = -p2y; }
	p2y += clickpos[1];
	for (int i = 0; i < 30; i++) {
		if (activeBird[i] == 0) {
			totalActiveBirds++;
			activeBird[i] = 1;
			birds[i].formula.X2 = clickpos[0];
			birds[i].formula.Y2 = clickpos[1];
			birds[i].formula.A = (p1y - birds[i].formula.Y2) / pow((-1 - birds[i].formula.X2), 2);
			birds[i].formula.B = (p2y - birds[i].formula.Y2) / pow((1 - birds[i].formula.X2), 2);
			birds[i].location.x = -1.1;
			birds[i].location.y = p1y;
			birds[i].dx = ((((float)rand() / RAND_MAX) * 0.02f) + 0.0055f);
			birds[i].theta = 0;
			break;
		}
	}
}

void lightning() {
	lightningSpawn = framesPassed;
	lightningPoints[4] = 0.53;
	int multiplyer = 1;
	if (rand() % 2 == 1) { multiplyer = -1; }
	for (int i = 3; i >= 0; i--) {
		lightningPoints[i] = ((((float)rand() / RAND_MAX) * 0.25f) + 0.1f) *(pow(-1,i)*multiplyer) + lightningPoints[i + 1];
	}
	

}

void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
			TEMPLATE: Add any new character key controls here.

			Rather than using literals (e.g. "d" for diagnostics), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
		*/
	case KEY_Q:
		exit(0);
		break;
	case KEY_S:
		if (snowfall == true) { snowfall = false; }
		else { snowfall = true; }
		break;
	case KEY_F:
		if (fire == true) { fire = false; }
		else { fire = true; lightning(); }
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
		
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}
void calculateFlame(float x2, float y2, float x3, float y3) {
	fireEquation.x2 = x2;
	fireEquation.y2 = y2;
	fireEquation.A = (0.53 - fireEquation.x2) / pow((lanscape[145] - 0.08 - fireEquation.y2), 2);
	fireEquation.B = (0.532 - fireEquation.x2) / pow((lanscape[145] + 0.15 - fireEquation.y2), 2);
	fireEquation.x3 = x3;
	fireEquation.y3 = y3;
	fireEquation.A2 = (0.53 - fireEquation.x3) / pow((lanscape[145] - 0.08 - fireEquation.y3), 2);
	fireEquation.B2 = (0.528 - fireEquation.x3) / pow((lanscape[145] + 0.15 - fireEquation.y3), 2);
}
/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */

void init(void)
{
	srand((unsigned)time(NULL));

	for (int i = 0; i <= totalSnow; i++) {
		snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
		snow[i].location.y = 1.0f;
		snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;
		snow[i].dy = ((((float)rand() / RAND_MAX) * 0.005f) + 0.01f) * snow[i].size;
		snow[i].landTime = 0;
		snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
		snow[i].active = true;
		snow[i].transparancy = ((((float)rand() / RAND_MAX) * 0.2f) + 0.6f);
	}
	for (int i = totalSnow + 1; i <= 5000; i++) {
		snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
		snow[i].location.y = 1.05f; // off render untill active
		snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;
		snow[i].dy = 0; // initial velocity to 0 untill activated
		snow[i].landTime = 0; //shows it has not landed yet
		snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
		snow[i].active = false;
		snow[i].transparancy = ((((float)rand() / RAND_MAX) * 0.2f) + 0.6f);
	}

	//generating random lanscape where it is random but dosent have any to steep changes by comparing heigh to previous height
	lanscape[0] = (((float)rand() / RAND_MAX) * 0.3f) - 0.8f;
	snowHeight[0][0] = snowHeight[1][0] = snowHeight[2][0] = lanscape[0];
	for (int i = 1; i < 200; i++) {
		lanscape[i] = (((float)rand() / RAND_MAX) * 0.02f) - 0.01 + lanscape[i - 1];
		if (lanscape[i] > -0.4) {
			lanscape[i] = -0.4;
		}
		else if (lanscape[i] < -0.75) {
			lanscape[i] = -0.75;
		}
		snowHeight[0][i] = snowHeight[1][i] = snowHeight[2][i] = snowHeight[3][i] = lanscape[i] - 0.003;
	}
	calculateFlame(0.53 - 0.1, lanscape[145] + 0.07, 0.53 + 0.1, lanscape[145] + 0.07);
	fireEquation.state = 0;
}

void think(void)
{
	//srand((unsigned)time(NULL));
	framesPassed++;

	if (snowfall) {
		if (totalSnow != 5000) {
			for (int i = 0; i <= 5000; i++) {
				if (snow[i].active == false) {
					totalSnow++;
					snow[i].active = true;
					snow[i].dy = ((((float)rand() / RAND_MAX) * 0.005f) + 0.01f) * snow[i].size;
					break;
				}
			}
		}
	}

	//angle += 0.01;
	for (int i = 0; i <= 5000; i++) {
		if (snow[i].active) {
			snow[i].location.y -= snow[i].dy * FRAME_TIME_SEC;
			int heightIndex = round((snow[i].location.x + 1) * 100);
			if (heightIndex > 199) { heightIndex = 199; } // stops error where they round to the next one and go lower than the lanscape
			if (snow[i].location.y - (snow[i].size / FramePixels) < snowHeight[snow[i].depth][heightIndex] && snow[i].landTime == 0) {
				snow[i].location.y = snowHeight[snow[i].depth][heightIndex]+ (snow[i].size / FramePixels);
				snow[i].landTime = framesPassed;
				snowHeight[snow[i].depth][heightIndex] += snow[i].size / FramePixels;
				snow[i].dy = 0;
				if (fire == true && lightningSpawn + 6 < framesPassed) {
					if (snow[i].location.x >= 0.33 && snow[i].location.x <= 0.73) {
						snow[i].lifetime = 0;
					}
					else if (snow[i].location.x > -0.42 && snow[i].location.x <= 0.33) {
						snow[i].lifetime = (rand() % 3500 + 2000)* (snow[i].location.x-0.38) *-1.5;
					}
					else if (snow[i].location.x >= 0.73) {
						snow[i].lifetime = (rand() % 3500 + 2000) * ((snow[i].location.x - 0.68) * 1.5);
					}else{ 
						snow[i].lifetime = rand() % 3500 + 2000; 
					}
				}else{ 
					snow[i].lifetime = rand() % 3500 + 2000; 
				}
				continue;
			}
			else if (snow[i].dy == 0 && (snow[i].location.y) > snowHeight[snow[i].depth][heightIndex]) {
				snow[i].location.y = snowHeight[snow[i].depth][heightIndex];
			}
			if (framesPassed > snow[i].landTime + snow[i].lifetime && snow[i].landTime != 0) {
				for (int x = 0; x < 5000; x++) {
					if ((round((snow[x].location.x + 1) * 100) == round((snow[i].location.x + 1) * 100) && snow[x].landTime != 0) && snow[x].location.y > snow[i].location.y && snow[x].depth == snow[i].depth) {
						snow[x].location.y -= snow[i].size / FramePixels;
					}
				}
				snowHeight[snow[i].depth][heightIndex] -= snow[i].size / FramePixels;
				snow[i].landTime = 0;
				snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
				snow[i].location.y = 1.05f;
				snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;
				snow[i].transparancy = ((((float)rand() / RAND_MAX) * 0.2f) + 0.6f);
				snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
				if (snowfall == true && true != (fire == true && lightningSpawn + 7 == framesPassed)) {
					snow[i].dy = ((((float)rand() / RAND_MAX) * 0.005f) + 0.01f) * snow[i].size;
				}
				else {
					snow[i].active = false;
					snow[i].dy = 0;
					totalSnow--;
				}
			}
			if (fire == true && lightningSpawn + 6 == framesPassed) {
				if (snow[i].landTime != 0) {
					if (snow[i].location.x >= 0.33 && snow[i].location.x <= 0.73) {
						snow[i].lifetime = 0;
						snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
						snow[i].location.y = 1.05f;
						snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;
						snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
						snow[i].transparancy = ((((float)rand() / RAND_MAX) * 0.2f) + 0.6f);
						snow[i].active = false;
						snow[i].dy = 0;
						totalSnow--;
					}
					else if (snow[i].location.x > -0.42 && snow[i].location.x <= 0.33) { snow[i].lifetime = (snow[i].lifetime * ((snow[i].location.x - 0.38) * -1.5)); }
					else if (snow[i].location.x >= 0.73) { snow[i].lifetime = (snow[i].lifetime * ((snow[i].location.x - 0.68) * 1.5)); }
					if (framesPassed > snow[i].landTime + snow[i].lifetime) {
						
					}
				}
				for (int x = 133; x <= 173; x ++) {
					snowHeight[0][x] = lanscape[x] - 0.003;
					snowHeight[1][x] = lanscape[x] - 0.003;
					snowHeight[2][x] = lanscape[x] - 0.003;

				}

			}
			if (snow[i].location.y - (snow[i].size / FramePixels) < snowHeight[3][heightIndex]) {
				snow[i].location.y = snowHeight[3][heightIndex] + (snow[i].size / FramePixels);
				snow[i].location.y = snowHeight[snow[i].depth][heightIndex]+ (snow[i].size / FramePixels);
				snow[i].landTime = framesPassed;
				snowHeight[snow[i].depth][heightIndex] += snow[i].size / FramePixels;
				snow[i].dy = 0;
				if (fire == true && lightningSpawn + 6 < framesPassed) {
					if (snow[i].location.x >= 0.33 && snow[i].location.x <= 0.73) {
						snow[i].lifetime = 0;
					}
					else if (snow[i].location.x > -0.42 && snow[i].location.x <= 0.33) {
						snow[i].lifetime = (rand() % 3500 + 2000) * (snow[i].location.x - 0.38) * -1.5;
					}
					else if (snow[i].location.x >= 0.73) {
						snow[i].lifetime = (rand() % 3500 + 2000) * ((snow[i].location.x - 0.68) * 1.5);
					}
					else {
						snow[i].lifetime = rand() % 3500 + 2000;
					}
				}
				if(snowHeight[snow[i].depth][heightIndex] < snow[i].location.y){ snowHeight[snow[i].depth][heightIndex] += snow[i].location.y;}
			}

		}
	}
	

	for (int i = 0; i < 80; i++) {
		if (activeBird[i] == 1) {
			birds[i].location.x += birds[i].dx;
			float gradient;
			if (birds[i].location.x > 1.1) { activeBird[i] = 0; totalActiveBirds--; continue; }
			if (birds[i].location.x <= birds[i].formula.X2) {
				birds[i].location.y = birds[i].formula.A * pow((birds[i].location.x - birds[i].formula.X2), 2) + birds[i].formula.Y2;
				gradient = 2 * birds[i].formula.A * (birds[i].location.x - birds[i].formula.X2);
			}
			else {
				birds[i].location.y = birds[i].formula.B * pow((birds[i].location.x - birds[i].formula.X2), 2) + birds[i].formula.Y2;
				gradient = 2 * birds[i].formula.B * (birds[i].location.x - birds[i].formula.X2);
			}
			birds[i].theta = atan(gradient)*180/M_PI;
			if (i != 0) {
			}
		}
	}
	
	if (fireEquation.state == 0) {
		if (fireEquation.y2 < lanscape[145] + 0.11) {calculateFlame(fireEquation.x2, fireEquation.y2 + 0.001, fireEquation.x3, fireEquation.y3 - 0.001);}
		else {fireEquation.state = 1;}
	}
	else if (fireEquation.state == 1) {
		if (fireEquation.x2 < 0.48){ calculateFlame(fireEquation.x2+0.01, fireEquation.y2, fireEquation.x3-0.01, fireEquation.y3); }
		else { fireEquation.state = 2; }
	}
	else if (fireEquation.state == 2) {
		if (fireEquation.x3 <= 0.63) { calculateFlame(fireEquation.x2 - 0.01, lanscape[145] - 0.04, fireEquation.x3 +0.01, lanscape[145] + 0.11); }
		else { fireEquation.state = 0; }
	}
	else if (fireEquation.state == 3) {
		if (fireEquation.y3 > lanscape[145] -0.04) { calculateFlame(fireEquation.x2, fireEquation.y2 - 0.001, fireEquation.x3, fireEquation.y3 + 0.001); }
		else { fireEquation.state = 4; }
	}
	else if (fireEquation.state == 4) {
		if (fireEquation.x2 > 0.5) { calculateFlame(fireEquation.x2 - 0.01, fireEquation.y2, fireEquation.x3 = 0.01, fireEquation.y3); }
		else { fireEquation.state = 5; }
	}
	else {
		if (fireEquation.x3 < 0.63) { calculateFlame(fireEquation.x2 + 0.01, lanscape[145] - 0.04, fireEquation.x3 - 0.01, lanscape[145] + 0.011); }
		else { fireEquation.state = 0; }
	}
}
//calculateFlame(0.53 - 0.1, lanscape[145] + 0.05, 0.53 + 0.1, lanscape[145] + 0.05);