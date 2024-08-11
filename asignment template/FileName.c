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

#define KEY_Q			113 // q key.
#define KEY_S			115 // q key.

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);
void mouse(int button, int state, int x, int y);
void printText(char text[], float x, float y);
void circle(float radius, float x, float y, float centerX, float centerY, float centerColor[3], float outerColor[3], float startPoint, float endPoint, bool background);
void rotate(float angle, float xInitial, float yInitial, float* xFinal, float* yFinal);
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
	bool active;
}Partical;

typedef struct {
	Position2 location;
	Quadric formula;
	float dx;
}bird;

Partical  snow[10001];
float lanscape[200];
float snowHeight[3][200];

bird birds[31];
int activeBird[31];
int totalActiveBirds = 0;
float angle = M_PI / 4;

int framesPassed = 1;
int totalSnow = 50;
bool snowfall = true;
GLfloat clickpos[2] = { 0,0 };
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

	glBegin(GL_QUAD_STRIP); // sky to be invert of the ground
	glColor3f(0.647, 0.898, 0.9686274);
	int num = 0;
	for (float i = -1; i < 1; i += 0.0101) {
		glVertex2f(i, 1);
		glVertex2f(i, lanscape[num]);
		num++;
	}
	glEnd();
	bool firstPass; // variable so that you can run some things only once to improve eficency for example background calculation in circle function
	if (framesPassed == 1) { firstPass = true; }
	else { firstPass = false; }

	glColor3f(1, 1, 1);
	for (int i = 0; i < 10000; i++) {
		if (snow[i].depth <= 1 && snow[i].active) {
			glPointSize(snow[i].size);
			glBegin(GL_POINTS);
			glVertex2f(snow[i].location.x, snow[i].location.y);
			glEnd();
		}
	}

	glColor3f(0.298, 0.6902, 0.0196);
	glBegin(GL_QUAD_STRIP);
	num = 0;
	for (float i = -1; i < 1; i += 0.0101) {
		glVertex2f(i, -1);
		glVertex2f(i, lanscape[num]);
		num++;
	}
	glEnd();



	float snowmanCenterColor[3] = { 1,1,1 };
	float snowmanCuterColor[3] = { 0.6902, 0.83137, 0.8196 };
	circle(0.15, -0.5, lanscape[100] + 0.1, -0.5, lanscape[100] + 0.1, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.12, -0.5, lanscape[100] + 0.36, -0.5, lanscape[100] + 0.36, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.07, -0.5, lanscape[100] + 0.54, -0.5, lanscape[100] + 0.54, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);

	// makes snow of depth level 2 render infront of objects
	glColor3f(1, 1, 1);
	for (int i = 0; i < 10000; i++) {
		if (snow[i].depth == 2 && snow[i].active) {
			glPointSize(snow[i].size);
			glBegin(GL_POINTS);
			glVertex2f(snow[i].location.x, snow[i].location.y);
			glEnd();
		}
	}


	glColor3f(0.298, 0, 0);

	char text[] = "bird";
	for (int i = 0; i < 4; i++) {
		glRasterPos2f(-0.5, 0.8);

		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, "h");
	}


	for (int i = 0; i < 30; i++) {
		if (activeBird[i] == 1) {
			float x, y;
			float birdCenterColor[3] = { 0.2118, 0.1529, 0 };
			float birdOuterColor[3] = { 0.2118, 0.1529, 0 };
			glColor3f(0.2118, 0.1529, 0);


			glBegin(GL_POLYGON);
			rotate(0, 0.035355, 0.017678, &x, &y);
			glVertex2f(birds[i].location.x + x, birds[i].location.y + y);
			glVertex2f(birds[i].location.x - x, birds[i].location.y + y);
			glVertex2f(birds[i].location.x - x, birds[i].location.y - y);
			glVertex2f(birds[i].location.x + x, birds[i].location.y - y);
			glEnd();

			glBegin(GL_POLYGON);
			rotate(0, 0.008445, 0.035606602, &x, &y);
			glVertex2f(birds[i].location.x + x, birds[i].location.y + y);
			rotate(0, -0.030445, 0.035606602, &x, &y);
			glVertex2f(birds[i].location.x + x, birds[i].location.y + y);
			rotate(0, -0.030445, 0.014393398, &x, &y);
			glVertex2f(birds[i].location.x + x, birds[i].location.y + y);
			rotate(0, 0.008445, 0.014393398, &x, &y);
			glVertex2f(birds[i].location.x + x, birds[i].location.y + y);
			glEnd();

			glBegin(GL_TRIANGLES);
			glVertex2f(birds[i].location.x + 0.005, birds[i].location.y + 0.045);
			glVertex2f(birds[i].location.x + 0.005, birds[i].location.y + 0.015);
			glVertex2f(birds[i].location.x + 0.02, birds[i].location.y + 0.015);
			glEnd();

			glBegin(GL_TRIANGLES);
			glVertex2f(birds[i].location.x - 0.035, birds[i].location.y + 0.03);
			glVertex2f(birds[i].location.x + 0.01, birds[i].location.y + 0.03);
			glVertex2f(birds[i].location.x + 0.01, birds[i].location.y + 0.063);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex2f(birds[i].location.x + 0.035, birds[i].location.y + 0.01);
			glVertex2f(birds[i].location.x + 0.06, birds[i].location.y + 0.01);
			glVertex2f(birds[i].location.x + 0.06, birds[i].location.y + 0.02);
			glVertex2f(birds[i].location.x + 0.035, birds[i].location.y + 0.02);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex2f(birds[i].location.x - 0.05, birds[i].location.y + 0.0025);
			glVertex2f(birds[i].location.x - 0.0625, birds[i].location.y + 0.0025);
			glVertex2f(birds[i].location.x - 0.0625, birds[i].location.y + 0.015);
			glVertex2f(birds[i].location.x + 0.05, birds[i].location.y + 0.015);
			glEnd();

			glBegin(GL_POLYGON);
			glVertex2f(birds[i].location.x - 0.05, birds[i].location.y + 0.02);
			glVertex2f(birds[i].location.x - 0.06, birds[i].location.y + 0.02);
			glVertex2f(birds[i].location.x - 0.06, birds[i].location.y + 0.015);
			glVertex2f(birds[i].location.x + 0.05, birds[i].location.y + 0.015);
			glEnd();

			glBegin(GL_TRIANGLES);
			glVertex2f(birds[i].location.x - 0.045, birds[i].location.y + 0.02);
			glVertex2f(birds[i].location.x - 0.065, birds[i].location.y + 0.0175);
			glVertex2f(birds[i].location.x - 0.065, birds[i].location.y + 0.0275);
			glEnd();


			glColor3f(0, 0, 0);
			glBegin(GL_TRIANGLES);
			glVertex2f(birds[i].location.x + 0.06, birds[i].location.y + 0.01);
			glVertex2f(birds[i].location.x + 0.06, birds[i].location.y + 0.02);
			glVertex2f(birds[i].location.x + 0.07, birds[i].location.y + 0.012);
			glEnd();
			glColor3f(0.2118, 0.1529, 0);

			circle(0.005, birds[i].location.x - 0.0625, birds[i].location.y + 0.007375, birds[i].location.x - 0.0625, birds[i].location.y + 0.007375, birdCenterColor, birdOuterColor, 1 * M_PI, 2 * M_PI, false);
			circle(0.00375, birds[i].location.x - 0.065, birds[i].location.y + 0.02375, birds[i].location.x - 0.065, birds[i].location.y + 0.02375, birdCenterColor, birdOuterColor, 1 * M_PI, 2 * M_PI, false);
			circle(0.0275, birds[i].location.x - 0.005, birds[i].location.y + (0.035 / sin(M_PI / 4)) * sin(angle), birds[i].location.x + (0.005 / sin(M_PI / 4)) * cos(angle), birds[i].location.y + (0.03 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1.25 * M_PI, 2.15 * M_PI, false);
			circle(0.03, birds[i].location.x - 0.035, birds[i].location.y + 0.05, birds[i].location.x - 0.05, birds[i].location.y + 0.01, birdCenterColor, birdOuterColor, 1 * M_PI, 1.17 * M_PI, false);
			circle(0.3, birds[i].location.x + (0.015 / sin(M_PI / 4)) * cos(angle), birds[i].location.y + (0.277 / sin(M_PI / 4)) * sin(angle), birds[i].location.x + (0.015 / sin(M_PI / 4)) * cos(angle), birds[i].location.y + (-0.015 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1 * M_PI - M_PI / 4 + angle, 1.055 * M_PI - M_PI / 4 + angle, false);
			circle(0.052, birds[i].location.x - (0.00 / sin(M_PI / 4)) * cos(angle), birds[i].location.y + (0.022 / sin(M_PI / 4)) * sin(angle), birds[i].location.x - (0.03 / sin(M_PI / 4)) * cos(angle), birds[i].location.y + (0.018 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1.2 * M_PI - M_PI / 4 + angle, 1.46 * M_PI - M_PI / 4 + angle, false);
			circle(0.02, birds[i].location.x + 0.056569 * cos(angle), birds[i].location.y + (0.018 / sin(M_PI / 4)) * sin(angle), birds[i].location.x + (0.04 / sin(M_PI / 4)) * cos(angle), birds[i].location.y + (0.015 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1.45 * M_PI, 2.55 * M_PI, false);
			circle(0.055, birds[i].location.x + 0.120202 * cos(angle), birds[i].location.y - (0.04 / sin(M_PI / 4)) * sin(angle), birds[i].location.x + (0.035 / sin(M_PI / 4)) * cos(angle), birds[i].location.y + (0.01 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, 1.625 * M_PI - M_PI / 4 + angle, 1.9 * M_PI - M_PI / 4 + angle, false);
			circle(0.02, birds[i].location.x + (0.015 / sin(M_PI / 4)) * cos(angle), birds[i].location.y - (0.003 / sin(M_PI / 4)) * sin(angle), birds[i].location.x + (0.015 / sin(M_PI / 4)) * cos(angle), birds[i].location.y - (0.015 / sin(M_PI / 4)) * sin(angle), birdCenterColor, birdOuterColor, M_PI - M_PI / 4, 1 * M_PI - M_PI / 4 + angle, false);
			float eyeColour[3] = { 1,1,1 };
			circle(0.004, birds[i].location.x + 0.045, birds[i].location.y + 0.025, birds[i].location.x + 0.045, birds[i].location.y + 0.025, eyeColour, eyeColour, 0 * M_PI, 2 * M_PI, false);


			
		}
	}
	glColor3f(1, 0, 0);
	printText("Number of Snow Particles:", -0.95, 0.85); // prints the snow amount
	for (int i = 0; i < 4; i++) {
		glRasterPos2f(-0.95 + 0.0225 * (strlen("Number of Snow Particles:") + i), 0.85);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (totalSnow / (int)pow(10, 3 - i)) % 10 + 48);
	}
	printText("/10000", -0.95 + (strlen("Number of Snow Particles:4000")) * 0.0225, 0.85);

	printText("Number of Birds:", -0.95, 0.8);// prints the bird amount
	for (int i = 0; i < 2; i++) {
		glRasterPos2f(-0.95 + 0.0225 * (strlen("Number of Birds:") + i), 0.8);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, (totalActiveBirds / (int)pow(10, 1 - i)) % 10 + 48);
	}
	printText("/80", -0.95 + (strlen("Number of Birds:10")) * 0.0225, 0.8);

	printText("Press q to exit", -0.95, 0.75);// prints the comands
	printText("Press s to stop snow", -0.95, 0.7);// prints the comands
	printText("Click to summon bird", -0.95, 0.65);// prints the comands

	
	float logCenterColor[3] = { 0.6392, 0.3412, 0.0627 };
	float logOuterColor[3] = { 0.388235, 0.2, 0.0235 };
	circle(0.05, 0.5, -0.76, 0.46, -0.74, logCenterColor, logOuterColor, 1.5 * M_PI, 1.815 * M_PI, true);
	glBegin(GL_QUAD_STRIP);
		glColor3f(0.388235, 0.2, 0.0235);
		glVertex2f(0.63, -0.8); //
		glVertex2f(0.45, -0.76);
		glColor3f(0.6392, 0.3412, 0.0627);
		glVertex2f(0.64, -0.775); //
		glVertex2f(0.46, -0.74);
		glColor3f(0.388235, 0.2, 0.0235);
		glVertex2f(0.65, -0.75);
		glVertex2f(0.47, -0.72);
	glEnd();
	circle(0.023, 0.558, -0.716, 0.56, -0.705, logCenterColor, logOuterColor, 1.7 * M_PI, 2.45 * M_PI, true);
	glBegin(GL_QUAD_STRIP);
		glColor3f(0.388235, 0.2, 0.0235);
		glVertex2f(0.43, -0.845);
		glVertex2f(0.54, -0.7);
		glColor3f(0.6392, 0.3412, 0.0627);
		glVertex2f(0.455, -0.8525);
		glVertex2f(0.56, -0.705);
		glColor3f(0.388235, 0.2, 0.0235);
		glVertex2f(0.48, -0.86);
		glVertex2f(0.58, -0.71);
	glEnd();
	circle(0.0235, 0.498, -0.713, 0.49, -0.7, logCenterColor, logOuterColor, 1.5 * M_PI, 2.1 * M_PI, true);
	glBegin(GL_QUAD_STRIP);
		glColor3f(0.388235, 0.2, 0.0235);
		glVertex2f(0.6, -0.87);
		glVertex2f(0.475, -0.71);
		glColor3f(0.6392, 0.3412, 0.0627);
		glVertex2f(0.6225, -0.86);
		glVertex2f(0.49, -0.7);
		glColor3f(0.388235, 0.2, 0.0235);
		glVertex2f(0.645, -0.85);
		glVertex2f(0.505, -0.69);
	glEnd();
	circle(0.0235, 0.522, -0.72, 0.5175, -0.705, logCenterColor, logOuterColor, 1.60 * M_PI, 2.2 * M_PI, true);
	glBegin(GL_QUAD_STRIP);
		glColor3f(0.388235, 0.2, 0.0235);
		glVertex2f(0.51, -0.89);
		glVertex2f(0.50, -0.71);
		glColor3f(0.6392, 0.3412, 0.0627);
		glVertex2f(0.535, -0.885);
		glVertex2f(0.5175, -0.705);
		glColor3f(0.388235, 0.2, 0.0235);
		glVertex2f(0.56, -0.88);
		glVertex2f(0.535, -0.7);
	glEnd();
	

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

void circle(float radius, float x, float y, float centerX, float centerY, float centerColor[3], float outerColor[3], float startPoint, float endPoint, bool background) {
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(centerColor[0], centerColor[1], centerColor[2]);
	glVertex2f(centerX, centerY);
	glColor3f(outerColor[0], outerColor[1], outerColor[2]);
	if (background == false) {
		for (float i = startPoint; i < endPoint; i += 0.01) {
			glVertex2f(x + radius * sin(i), y + radius * cos(i));
		}

	}
	else {
		for (float i = startPoint; i < endPoint; i += 0.01) {
			glVertex2f(x + radius * sin(i), y + radius * cos(i));
			int heightIndex = round((x + radius * sin(i) + 1) * 100);
			if (snowHeight[1][heightIndex] < y + radius * cos(i)) {
				snowHeight[1][heightIndex] = y + radius * cos(i);
			}
			if (snowHeight[2][heightIndex] > y + radius * cos(i)) {
				snowHeight[2][heightIndex] = y + radius * cos(i);
			}
		}
	}
	glEnd();
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
			break;
		}
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
		//printf("skip");
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
	}
	for (int i = totalSnow + 1; i <= 10000; i++) {
		snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
		snow[i].location.y = 1.05f; // off render untill active
		snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;
		snow[i].dy = 0; // initial velocity to 0 untill activated
		snow[i].landTime = 0; //shows it has not landed yet
		snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
		snow[i].active = false;
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
		snowHeight[0][i] = snowHeight[1][i] = snowHeight[2][i] = lanscape[i] - 0.003;
	}

	/*//test to devlop bird
	birds[0].formula.X2 = 0;
	birds[0].formula.Y2 = 0.3;
	birds[0].formula.A = 1;
	birds[0].formula.B = 1;
	birds[0].location.x = 0;
	birds[0].location.y = 0.3;
	birds[0].dx = 0;
	activeBird[0] = 1;*/

}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	//srand((unsigned)time(NULL));
	framesPassed++;

	if (snowfall) {
		if (totalSnow != 10000) {
			for (int i = 0; i <= 10000; i++) {
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
	for (int i = 0; i <= 10000; i++) {
		if (snow[i].active) {
			snow[i].location.y -= snow[i].dy * FRAME_TIME_SEC;
			int heightIndex = round((snow[i].location.x + 1) * 100);
			if (heightIndex > 199) { heightIndex = 199; } // stops error where they round to the next one and go lower than the lanscape
			if (snow[i].location.y - (snow[i].size / FramePixels) < snowHeight[snow[i].depth][heightIndex] && snow[i].landTime == 0) {
				snow[i].landTime = framesPassed;
				snowHeight[snow[i].depth][heightIndex] += snow[i].size / FramePixels;
				snow[i].dy = 0;
				snow[i].lifetime = rand() % 3500 + 2000;
				continue;
			}
			else if (snow[i].dy == 0 && (snow[i].location.y) > snowHeight[snow[i].depth][heightIndex]) {
				snow[i].location.y = snowHeight[snow[i].depth][heightIndex];
			}
			if (framesPassed > snow[i].landTime + snow[i].lifetime && snow[i].landTime != 0) {
				for (int x = 0; x < 10000; x++) {
					if ((round((snow[x].location.x + 1) * 100) == round((snow[i].location.x + 1) * 100) && snow[x].landTime != 0) && snow[x].location.y > snow[i].location.y && snow[x].depth == snow[i].depth) {
						snow[x].location.y -= snow[i].size / FramePixels;
					}
				}
				snowHeight[snow[i].depth][heightIndex] -= snow[i].size / FramePixels;
				snow[i].landTime = 0;
				snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
				snow[i].location.y = 1.05f;
				snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;

				snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
				if (snowfall == true) {
					snow[i].dy = ((((float)rand() / RAND_MAX) * 0.005f) + 0.01f) * snow[i].size;
				}
				else {
					snow[i].active = false;
					snow[i].dy = 0;
					totalSnow--;
				}
			}
		}
	}

	for (int i = 0; i < 80; i++) {
		if (activeBird[i] == 1) {
			birds[i].location.x += birds[i].dx;
			if (birds[i].location.x > 1.1) { activeBird[i] = 0; totalActiveBirds--; continue; }
			if (birds[i].location.x <= birds[i].formula.X2) {
				birds[i].location.y = birds[i].formula.A * pow((birds[i].location.x - birds[i].formula.X2), 2) + birds[i].formula.Y2;
			}
			else {
				birds[i].location.y = birds[i].formula.B * pow((birds[i].location.x - birds[i].formula.X2), 2) + birds[i].formula.Y2;
			}
		}
	}

}