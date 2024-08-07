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

#define KEY_EXIT			27 // Escape key.

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);
void mouse(int button, int state, int x, int y);

void circle(float radius, float x, float y, float centerX, float centerY, float centerColor[3], float outerColor[3], float startPoint, float endPoint, bool background);

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

typedef struct{
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
}Partical;

typedef struct {
	Position2 location;
	Quadric formula;
	float dx;
}bird;

Partical  snow[10000];
float lanscape[200];
float snowHeight[3][200];

bird birds[50];
int activeBird[50];


int framesPassed = 1;
int activeSnow = 50;
float angle = 0;
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

	bool firstPass; // variable so that you can run some things only once to improve eficency for example background calculation in circle function
	if (framesPassed == 1) { firstPass = true; }
	else { firstPass = false; }
	glClearColor(0.647, 0.898, 0.9686274, 1);
	glColor3f(1, 1, 1);
	for (int i = 0; i < activeSnow; i++) {
		if (snow[i].depth <= 1) {
			glPointSize(snow[i].size);
			glBegin(GL_POINTS);
			glVertex2f(snow[i].location.x, snow[i].location.y);
			glEnd();
		}
	}

	glColor3f(0.298, 0.6902, 0.0196);
	glBegin(GL_QUAD_STRIP);
	int num = 0;
	for (float i = -1; i < 1; i += 0.0101) {
		glVertex2f(i, -1);
		glVertex2f(i, lanscape[num]);
		num++;
	}
	glEnd();

	float snowmanCenterColor[3] = { 1,1,1 };
	float snowmanCuterColor[3] = { 0.6902, 0.83137, 0.8196 };
	circle(0.15, 0, lanscape[100] + 0.1, 0, lanscape[100] + 0.1, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.12, 0, lanscape[100] + 0.36, 0, lanscape[100] + 0.36, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);
	circle(0.07, 0, lanscape[100] + 0.54, 0, lanscape[100] + 0.54, snowmanCenterColor, snowmanCuterColor, 0, 2 * M_PI, firstPass);

	// makes snow of depth level 2 render infront of objects
	glColor3f(1, 1, 1);
	for (int i = 0; i < activeSnow; i++) {
		if (snow[i].depth == 2) {
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
	

	for (int i = 0; i < 50; i++) {
		if (activeBird[i] == 1) {
			glPointSize(10);
			glColor3f(0, 0, 0);
			
			/*glBegin(GL_POLYGON);
				glVertex2f(birds[i].location.x + 0.05 * cos(angle + M_PI/4), birds[i].location.y + 0.025 * sin(angle + M_PI / 4));
				glVertex2f(birds[i].location.x + 0.05 * cos(angle + M_PI/2 + M_PI / 4), birds[i].location.y + 0.025 * sin(angle + M_PI / 2 + M_PI / 4));
				glVertex2f(birds[i].location.x + 0.05 * cos(angle + M_PI + M_PI / 4), birds[i].location.y + 0.025 * sin(angle + M_PI + M_PI / 4));
				glVertex2f(birds[i].location.x + 0.05 * cos(angle + M_PI *1.5 + M_PI / 4), birds[i].location.y + 0.025 * sin(angle + M_PI*1.5 + M_PI / 4));
			glEnd();
			glBegin(GL_POLYGON);
			glVertex2f(birds[i].location.x + 0.05 * cos(angle + M_PI / 4), birds[i].location.y + 0.025 * sin(angle + M_PI / 4));
			glVertex2f(birds[i].location.x + 0.05 * cos(angle + M_PI / 2 + M_PI / 4), birds[i].location.y + 0.025 * sin(angle + M_PI / 2 + M_PI / 4));
			glVertex2f(birds[i].location.x + 0.05 * cos(angle + M_PI + M_PI / 4), birds[i].location.y + 0.025 * sin(angle + M_PI + M_PI / 4));
			glVertex2f(birds[i].location.x + 0.05 * cos(angle + M_PI * 1.5 + M_PI / 4), birds[i].location.y + 0.025 * sin(angle + M_PI * 1.5 + M_PI / 4));
			glEnd();*/
			glBegin(GL_LINES);
				glVertex2f(birds[i].location.x, birds[i].location.y );
				glVertex2f(birds[i].location.x + 0.05 * cos(angle), birds[i].location.y + 0.05 * sin(angle));
			glEnd();
			float birdCenterColor[3] = { 0,0,0};
			float birdCuterColor[3] = { 0, 0, 0 };
			circle(0.02, birds[i].location.x+0.04 * cos(angle), birds[i].location.y+0.018 * sin(angle), birds[i].location.x+0.04 * cos(angle), birds[i].location.y+0.018 * sin(angle), birdCenterColor, birdCuterColor, 1.5 * M_PI,2.5 * M_PI, false);
			circle(0.055, birds[i].location.x+0.085 * cos(angle), birds[i].location.y-0.04 * sin(angle), birds[i].location.x+0.035* cos(angle), birds[i].location.y+0.01*sin(angle), birdCenterColor, birdCuterColor, 1.625 * M_PI, 1.9 * M_PI, false);

		}
	}
	glutSwapBuffers();
}

void circle(float radius, float x, float y, float centerX, float centerY, float centerColor[3], float outerColor[3], float startPoint, float endPoint, bool background){
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
		clickpos[0]  = ((GLfloat)x / (GLfloat)FramePixels)*2-1;
		clickpos[1] = ((GLfloat)y / (GLfloat)FramePixels )*-2+1;
		birdfunc();
	}
}
void birdfunc(void) {
	float p1y = (((float)rand() / RAND_MAX) * 0.5f);
	p1y = p1y * (clickpos[0] + 1);
	if (rand() % 2 == 1) {p1y = -p1y;}
	p1y += clickpos[1];
	float p2y = (((float)rand() / RAND_MAX) * 0.5f);
	p2y = p2y * ((clickpos[0] - 1)*-1);
	if (rand() % 2 == 1) { p2y = -p2y; }
	p2y += clickpos[1];
	for (int i = 0; i < 50; i++) { 
		if (activeBird[i] == 0) { 
			activeBird[i] = 1;
			birds[i].formula.X2 = clickpos[0]; 
			birds[i].formula.Y2 = clickpos[1];
			birds[i].formula.A = (p1y - birds[i].formula.Y2) / pow((-1 - birds[i].formula.X2),2);
			birds[i].formula.B = (p2y - birds[i].formula.Y2) / pow((1 - birds[i].formula.X2), 2);
			birds[i].location.x = -1;
			birds[i].location.y = p1y;
			birds[i].dx = ((((float)rand() / RAND_MAX) * 0.05f) + 0.008f);
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
	case KEY_EXIT:
		exit(0);
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

	for (int i = 0; i <= activeSnow; i++) {
		snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
		snow[i].location.y = 1.0f;
		snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) +1.5f;
		snow[i].dy = ((((float)rand() / RAND_MAX) * 0.005f)+0.01f)* snow[i].size;
		snow[i].landTime = 0;
		snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
	}
	for (int i = activeSnow+1; i < 10000; i++) {
		snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
		snow[i].location.y = 1.05f; // off render untill active
		snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;
		snow[i].dy = 0; // initial velocity to 0 untill activated
		snow[i].landTime = 0; //shows it has not landed yet
		snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
	}

	//generating random lanscape where it is random but dosent have any to steep changes by comparing heigh to previous height
	lanscape[0] = (((float)rand() / RAND_MAX) * 0.3f) - 0.8f;
	snowHeight[0][0] = snowHeight[1][0] = snowHeight[2][0] = lanscape[0];
	for (int i = 1; i < 200; i++) {
		lanscape[i] = (((float)rand() / RAND_MAX) * 0.02f) - 0.01 +lanscape[i-1];
		if (lanscape[i] > -0.4) {
			lanscape[i] = -0.4;
		}
		else if (lanscape[i] < -0.75) {
			lanscape[i] = -0.75;
		}
		snowHeight[0][i] = snowHeight[1][i] = snowHeight[2][i] = lanscape[i]-0.003;
	}

	// test to devlop bird
	birds[0].formula.X2 = 0;
	birds[0].formula.Y2 = 0.3;
	birds[0].formula.A = 1;
	birds[0].formula.B = 1;
	birds[0].location.x = 0;
	birds[0].location.y = 0.3;
	birds[0].dx = 0;
	activeBird[0] = 1;
	 
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
	if (activeSnow < 10000 && framesPassed%2 == 0) {
		activeSnow++;
		snow[activeSnow].dy = ((((float)rand() / RAND_MAX) * 0.005f) + 0.01f) * snow[activeSnow].size;
	}
	angle += 0.01;
	for (int i = 0; i < activeSnow; i++) {
		snow[i].location.y -= snow[i].dy * FRAME_TIME_SEC;
		int heightIndex = round((snow[i].location.x+1) * 100);
		if (heightIndex > 199) { heightIndex = 199; } // stops error where they round to the next one and go lower than the lanscape
		if (snow[i].location.y - (snow[i].size / FramePixels) < snowHeight [snow[i].depth][heightIndex] && snow[i].landTime == 0) {
			snow[i].landTime = framesPassed;
			snowHeight[snow[i].depth][heightIndex] += snow[i].size / FramePixels;
			snow[i].dy = 0;
			snow[i].lifetime = rand() % 3500 + 2000;
			continue;
		}
		else if (snow[i].dy == 0 && (snow[i].location.y ) > snowHeight[snow[i].depth][heightIndex]) {
			snow[i].location.y = snowHeight[snow[i].depth][heightIndex];
		}
		if ( framesPassed >  snow[i].landTime + snow[i].lifetime && snow[i].landTime != 0) {
			for (int x = 0; x < activeSnow; x++) {
				if ((round((snow[x].location.x + 1) * 100) == round((snow[i].location.x + 1) * 100) && snow[x].landTime !=0) && snow[x].location.y > snow[i].location.y && snow[x].depth == snow[i].depth) {
					snow[x].location.y -= snow[i].size / FramePixels;
				}
			}
			snowHeight[snow[i].depth][heightIndex] -= snow[i].size / FramePixels;
			snow[i].landTime = 0;
			snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
			snow[i].location.y = 1.0f;
			snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;
			snow[i].dy = ((((float)rand() / RAND_MAX) * 0.005f) + 0.01f) * snow[i].size;
			snow[i].depth = rand() % 3; //sets layer to random 0,1 or 2
		}
	}

	for (int i = 0; i < 50; i++) {
		if (activeBird[i] == 1) {
			birds[i].location.x += birds[i].dx;
			if (birds[i].location.x > 1) { activeBird[i] = 0; continue;}
			if (birds[i].location.x <= birds[i].formula.X2) {
				birds[i].location.y = birds[i].formula.A * pow((birds[i].location.x - birds[i].formula.X2), 2) + birds[i].formula.Y2;
			}
			else {
				birds[i].location.y = birds[i].formula.B * pow((birds[i].location.x - birds[i].formula.X2), 2) + birds[i].formula.Y2;
			}
		}
	}
	
}
