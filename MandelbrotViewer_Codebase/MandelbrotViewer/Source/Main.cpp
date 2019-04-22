#include <GLFW/glfw3.h>
#include "MandelbrotSet.h"

//Constants
#define _SCREEN_WIDTH_MAX 1600
#define _SCREEN_HEIGHT_MAX 900
#define _SCREEN_WIDTH_MIN 1050
#define _SCREEN_HEIGHT_MIN 500

//Global variables
MandelbrotSet mandelbrotSet;
bool leftClickDown = false; //Is the mouse leftclick being pressed?
double leftClickMouseX, leftClickMouseY; //The x and y of the mouse when the mouse leftclicks
double mouseX, mouseY; //The current x and y position of the mouse

//If the left or right click has been pressed
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//If the left click has been pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS) 
		{
			leftClickDown = true;

			//Get the x and y positions of the mouse
			glfwGetCursorPos(window, &leftClickMouseX, &leftClickMouseY);
		}
		else if (action == GLFW_RELEASE)
		{
			leftClickDown = false;
		
			//Zoom into the set
			mandelbrotSet.ZoomIn(leftClickMouseX, leftClickMouseY, mouseX, mouseY);
		}
	}

	//If the right click has been pressed
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		mandelbrotSet.ZoomOut(2);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//'R' key resets the board
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		mandelbrotSet.Reset();
}

int main(void)
{
	//-----------------Initialize GLFW window------------------------------//
	double screenWidth = (_SCREEN_WIDTH_MAX + _SCREEN_WIDTH_MIN) / 2; //The current width of the window
	double screenHeight = (_SCREEN_HEIGHT_MAX + _SCREEN_HEIGHT_MIN) / 2; //The current height of the window

	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	//Create the window
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(screenWidth, screenHeight, "Mandelbrot Viewer", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//Set the min and max sizes for the window
	glfwSetWindowSizeLimits(window, _SCREEN_WIDTH_MIN, _SCREEN_HEIGHT_MIN, _SCREEN_WIDTH_MAX, _SCREEN_HEIGHT_MAX);

	//Make the new window appear in front of all other windows
	glfwMakeContextCurrent(window);

	//Event handlers
	glfwSetMouseButtonCallback(window, mouse_button_callback); //Mouseclick event
	glfwSetKeyCallback(window, key_callback); //Keyboard event

	//Tell the mandelbrot set how large the screen is
	mandelbrotSet.SetScreenSize(screenWidth, screenHeight);

	//Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//Get the x and y positions of the mouse
		glfwGetCursorPos(window, &mouseX, &mouseY);

		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0, 0, 0, 1); //Set the background color to all white

		//Draw the set
		mandelbrotSet.Draw();
		if (leftClickDown) //If the leftclick is down, draw the box selecting the new region to focus
		{
			mandelbrotSet.DrawSelectBox(leftClickMouseX, leftClickMouseY, mouseX, mouseY);
		}

		//Check if the window has been resized
		int newWidth, newHeight;
		glfwGetFramebufferSize(window, &newWidth, &newHeight);

		if (newWidth != screenWidth || newHeight != screenHeight)
		{
			screenWidth = newWidth;
			screenHeight = newHeight;

			mandelbrotSet.SetScreenSize(screenWidth, screenHeight);
		}

		//Double buffer
		glfwSwapBuffers(window);

		//Poll for events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}