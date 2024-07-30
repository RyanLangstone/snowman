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

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char** argv);
void init(void);
void think(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

typedef struct{
	float x;
	float y;

}Position2;

typedef struct {
	Position2 location;
	float size;
	float dy;
	int landTime;
	int lifetime;
}Partical;

Partical  snow[10000];
float lanscape[200];
float snowHeight[200];

int framesPassed = 1;


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

	glClearColor(0.647, 0.898, 0.9686274,1);
	glColor3f(1, 1, 1);
	if (framesPassed < 100) {
		for (int i = 0; i < framesPassed; i++) {
			glPointSize(snow[i].size);
			glBegin(GL_POINTS);
			glVertex2f(snow[i].location.x, snow[i].location.y);
			glEnd();
		}
	}
	else {
		for (int i = 0; i < 10000; i++) {
			glPointSize(snow[i].size);
			glBegin(GL_POINTS);
			glVertex2f(snow[i].location.x, snow[i].location.y);
			glEnd();
		}
	}

	glColor3f(0.298, 0.6902, 0.0196);
	glBegin(GL_QUAD_STRIP);
	int num = 0;
		for (float i = -1; i < 01; i += 0.0101) {
			glVertex2f(i, -1);
			glVertex2f(i, lanscape[num]);
			num++;
		}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		glColor3f(1, 1, 1);
		glVertex2f(0,-0.66);
		for (float x = 0; x < 2*M_PI; x +=0.01) {
			//glColor3f(0.819, 0.956, 1);
			glColor3f(0.6902, 0.83137, 0.8196);
			glVertex2f(-0.15 * sin(x), -0.66 + 0.15 * cos(x));
		}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		glColor3f(1, 1, 1);
		glVertex2f(0, -0.42);
		for (float x = 0; x < 2 * M_PI; x += 0.01) {
			//glColor3f(0.819, 0.956, 1);
			glColor3f(0.6902, 0.83137, 0.8196);
			glVertex2f(-0.12 * sin(x), -0.42 + 0.12 * cos(x));
		}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
		glColor3f(1, 1, 1);
		glVertex2f(0, -0.255);
		for (float x = 0; x < 2 * M_PI; x += 0.01) {
			//glColor3f(0.819, 0.956, 1);
			glColor3f(0.6902, 0.83137, 0.8196);
			glVertex2f(-0.07 * sin(x), -0.255 + 0.07 * cos(x));
		}
	glEnd();

	glutSwapBuffers();
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE

		Separate reusable pieces of drawing code into functions, which you can add
		to the "Animation-Specific Functions" section below.

		Remember to add prototypes for any new functions to the "Animation-Specific
		Function Prototypes" section near the top of this template.
	*/
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

	for (int i = 0; i < 10000; i++) {
		snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
		snow[i].location.y = 1.0f;
		snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) +1.5f;
		snow[i].dy = ((((float)rand() / RAND_MAX) * 0.005f)+0.01f)* snow[i].size;
		snow[i].landTime = 0;
	}

	//generating random lanscape where it is random but dosent have any to steep changes by comparing heigh to previous height
	lanscape[0] = (((float)rand() / RAND_MAX) * 0.3f) - 0.8f;
	snowHeight[0] = lanscape[0];
	for (int i = 1; i < 200; i++) {
		lanscape[i] = (((float)rand() / RAND_MAX) * 0.02f) - 0.01 +lanscape[i-1];
		if (lanscape[i] > -0.4) {
			lanscape[i] = -0.4;
		}
		else if (lanscape[i] < -0.75) {
			lanscape[i] = -0.75;
		}
		snowHeight[i] = lanscape[i];
	}
	 
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
	for (int i = 0; i < 10000; i++) {
		snow[i].location.y -= snow[i].dy * FRAME_TIME_SEC;
		int heightIndex = round((snow[i].location.x+1) * 100);
		if ((snow[i].location.y - (snow[i].size / FramePixels)) < snowHeight[heightIndex] && snow[i].landTime == 0) {
			snow[i].landTime = framesPassed;
			snowHeight[heightIndex] += snow[i].size / FramePixels;
			snow[i].dy = 0;
			snow[i].lifetime = (((int)rand() / RAND_MAX) * 5000) + 1000;
			continue;
		}
		else if (snow[i].dy == 0 && (snow[i].location.y ) > snowHeight[heightIndex]) {
			snow[i].location.y = snowHeight[heightIndex];
		}
		if ( framesPassed >  snow[i].landTime + snow[i].lifetime && snow[i].landTime != 0) {
			for (int x = 0; x < 10000; x++) {
				if ((round((snow[x].location.x + 1) * 100) == round((snow[i].location.x + 1) * 100) && snow[x].landTime !=0) && snow[x].location.y > snow[i].location.y) {
					snow[x].location.y -= snow[i].size / FramePixels;
				}
			}
			snowHeight[heightIndex] -= snow[i].size / FramePixels;
			snow[i].landTime = 0;
			snow[i].location.x = (((float)rand() / RAND_MAX) * 2.0f) - 1.0f;
			snow[i].location.y = 1.0f;
			snow[i].size = (((float)rand() / RAND_MAX) * 7.0f) + 1.5f;
			snow[i].dy = ((((float)rand() / RAND_MAX) * 0.005f) + 0.01f) * snow[i].size;
			
		}
	}
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE

		In this function, we update all the variables that control the animated
		parts of our simulated world. For example: if you have a moving box, this is
		where you update its coordinates to make it move. If you have something that
		spins around, here's where you update its angle.

		NOTHING CAN BE DRAWN IN HERE: you can only update the variables that control
		how everything will be drawn later in display().

		How much do we move or rotate things? Because we use a fixed frame rate, we
		assume there's always FRAME_TIME milliseconds between drawing each frame. So,
		every time think() is called, we need to work out how far things should have
		moved, rotated, or otherwise changed in that period of time.

		Movement example:
		* Let's assume a distance of 1.0 GL units is 1 metre.
		* Let's assume we want something to move 2 metres per second on the x axis
		* Each frame, we'd need to update its position like this:
			x += 2 * (FRAME_TIME / 1000.0f)
		* Note that we have to convert FRAME_TIME to seconds. We can skip this by
		  using a constant defined earlier in this template:
			x += 2 * FRAME_TIME_SEC;

		Rotation example:
		* Let's assume we want something to do one complete 360-degree rotation every
		  second (i.e. 60 Revolutions Per Minute, or RPM).
		* Each frame, we'd need to update our object's angle like this (we'll use the
		  FRAME_TIME_SEC constant as per the example above):
			a += 360 * FRAME_TIME_SEC;

		This works for any type of "per second" change: just multiply the amount you'd
		want to move in a full second by FRAME_TIME_SEC, and add or subtract that
		from whatever variable you're updating.

		You can use this same approach to animate other things like color, opacity,
		brightness of lights, etc.
	*/
}

/******************************************************************************/