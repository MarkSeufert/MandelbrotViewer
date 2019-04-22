#include <GLFW/glfw3.h>
#include "MandelbrotSet.h"
#include <cmath>

///Calculates whether a point is inside the mandelbrot set or not
///Returns a double between 0 and 1 where 0 means inside the set and 1 means outside the set
double MandelbrotSet::MandelbrotAlgorithm(double pReal, double pImaginary)
{
	double real = 0, imag = 0;
	double tmp, realSqr = 0, imagSqr = 0;
	for (double i = 0; i < mIterationMax; i++) { //This doesn't look like the mandelbrot algoritm as it is heavily modified to be more efficient
		imag = real * imag;
		imag += imag;
		imag += pImaginary;
		real = realSqr - imagSqr + pReal;
		realSqr = real * real;
		imagSqr = imag * imag;
		if (realSqr + imagSqr > 4) {//If the point is going to converge to infinity, stop it right away
			return 1 - i / mIterationMax;
		}
	}
	return 0; //The point is in the set
}

///Given the results from the mandelbrot set for that point, return a red, green, and blue colorvalues
void MandelbrotSet::ColoringAlgorithm(double pIteration, double &pRed, double &pGreen, double &pBlue)
{
	pIteration = pIteration * pIteration; //Square the pIteration to skew it towards 0. Do this to emphasize coloring near 0
	
	pRed = 1 - pIteration * 0.85;
	pGreen = 1 - pIteration * 0.8;
	pBlue = 1 - pIteration * pIteration / 2;
}

///Calculate all the points in the mandelbrot set that are visible on the screen and put it into memory
void MandelbrotSet::CalculateMandelbrotSet()
{
	//Calculate the number of sufficient max iterations at this scale. 
	mIterationMax = round(50 * (1 + pow(1 / mScale, 0.25)));

	double maxValue = 0;

	for (int column = 0; column < mWindowWidth; column++)
	{
		for (int row = 0; row < mWindowHeight; row++)
		{
			//Convert the iterators to real and imaginary coorinates
			double realComponent;
			double imaginaryComponent;
			ScreenToComplexPosition(column, row, realComponent, imaginaryComponent);

			//Compute the mandelbrot algorithm at that point
			double mandelResult = MandelbrotAlgorithm(realComponent, imaginaryComponent);

			//Save the results into the 2d mandelbrot memory
			mMandelbrotMemory[column][row] = mandelResult;

			//Update the max value
			if (mandelResult > maxValue)
				maxValue = mandelResult;
		}
	}

	//Normalize the 2d array between 0 and 1 using the maxValue
	for (int column = 0; column < mWindowWidth; column++)
	{
		for (int row = 0; row < mWindowHeight; row++)
		{
			mMandelbrotMemory[column][row] /= maxValue;
		}
	}
}

///Given an x and y pixel on the screen, return by reference the real and imaginary point associated with it
void MandelbrotSet::ScreenToComplexPosition(double pScreenX, double pScreenY, double &pReal, double &pImaginary)
{
	double mYScale = mWindowHeight / mWindowWidth * mScale; //The aspect ratio multiplied by the mScale

	pReal = mCenterReal - mScale + (2 * mScale) * pScreenX / mWindowWidth;
	pImaginary = mCenterImaginary + mYScale - (2 * mYScale) * pScreenY / mWindowHeight;
}

///Initialize several member variables to default values
MandelbrotSet::MandelbrotSet()
{
	mScale = 2.5;  //Initially have a range from (-2.5, 2.5) on the real axis
	mCenterReal = -0.5; //Shift this range from (-2.5, 2.5) into (-3.0, 2.0) on the real axis
	mCenterImaginary = -0.01; //Offset down slightly for minor graphics improvement
}

void MandelbrotSet::Reset()
{
	mScale = 2.5;  //Initially have a range from (-2.5, 2.5) on the real axis
	mCenterReal = -0.5; //Shift this range from (-2.5, 2.5) into (-3.0, 2.0) on the real axis
	mCenterImaginary = -0.01; //Offset down slightly for minor graphics improvement

	//Recalculate the mandelbrot set
	CalculateMandelbrotSet();
}

///Allocate memory to store the mandelbrot set for the new screen size, and then recalculate the set
void MandelbrotSet::SetScreenSize(int pWidth, int pHeight)
{
	//Deallocate any memory assoiciated with the mandelbrot set
	if (mMandelbrotMemory != NULL)
	{
		for (int i = 0; i < mWindowWidth; i++)
		{
			delete[] mMandelbrotMemory[i];
		}
		delete[] mMandelbrotMemory;
	}

	//Put the parameters into the member variables
	mWindowWidth = pWidth;
	mWindowHeight = pHeight;

	//Allocate memory for the mandelbrot set
	mMandelbrotMemory = new double*[mWindowWidth];
	for (int i = 0; i < mWindowWidth; i++) 
	{
		mMandelbrotMemory[i] = new double[mWindowHeight];
	}

	//Change the viewport to fit the new resolution
	glViewport(0, 0, mWindowWidth, mWindowHeight);

	//Recalculate the mandelbrot set
	CalculateMandelbrotSet();
}

///Called when the mandelbrot set is clicked on at a point and should zoom in/out from it
void MandelbrotSet::ZoomIn(double pStartX, double pStartY, double pEndX, double pEndY)
{
	//Make sure the x values and y values are within a sufficient distance from eachother
	if (abs(pStartX - pEndX) < 5 || abs(pStartY - pEndY) < 5)
		return;

	ScreenToComplexPosition((pStartX + pEndX) / 2, (pStartY + pEndY) / 2, mCenterReal, mCenterImaginary);

	double diffX = abs(pEndX - pStartX) / mWindowWidth;
	double diffY = abs(pEndY - pStartY) / mWindowHeight;

	//Choose the smaller difference and set zoom by that much
	if (diffX < diffY)
		mScale *= diffX;
	else
		mScale *= diffY;

	//Recalculate the mandelbrot set at the new zoom
	CalculateMandelbrotSet();
}

void MandelbrotSet::ZoomOut(double pZoom)
{
	mScale *= pZoom;

	//Recalculate the mandelbrot set at the new zoom
	CalculateMandelbrotSet();
}

///Draw the mandelbrot set from the mMandelbrotMemory array onto the screen
void MandelbrotSet::Draw()
{
	glLoadIdentity();
	glOrtho(0, mWindowWidth, mWindowHeight, 0, 0, 1);
	glColor3f(0.8f, 0.8f, 0.8f); //Set the color of the line to grey
	glPointSize(1); //Set the size of the pixel we are drawing to 1
	glBegin(GL_POINTS);

	//Iterate through all the points on the screen
	for (int column = 0; column < mWindowWidth; column++)
	{
		for (int row = 0; row < mWindowHeight; row++)
		{
			double mandelbrotValue = mMandelbrotMemory[column][row];
			if (mandelbrotValue != 0) {
				double red, green, blue;
				ColoringAlgorithm(mandelbrotValue, red, green, blue);
				glColor3f(red, green, blue);
				glVertex2f(column, row);
			}
		}
	}
	glEnd();
}

///Draw a white box around the area that the user is selecting
void MandelbrotSet::DrawSelectBox(double pStartX, double pStartY, double pEndX, double pEndY)
{
	//Make sure the x values and y values are within a sufficient distance from eachother
	if (abs(pStartX - pEndX) < 5 || abs(pStartY - pEndY) < 5)
		return;

	//Initialize openGl for drawing lines
	glLoadIdentity();
	glOrtho(0, mWindowWidth, mWindowHeight, 0, 0, 1);
	glLineWidth(1); //Set the width of the line to 2
	glColor3f(1, 1, 1); //Set the color of the line to white
	glBegin(GL_LINES);

	//Top/bottom of box
	glVertex2f(pStartX, pStartY);
	glVertex2f(pEndX, pStartY);

	//Left/right of box
	glVertex2f(pStartX, pStartY);
	glVertex2f(pStartX, pEndY);

	//Top/bottom of box
	glVertex2f(pStartX, pEndY);
	glVertex2f(pEndX, pEndY);

	//Left/right of box
	glVertex2f(pEndX, pStartY);
	glVertex2f(pEndX, pEndY);

	glEnd();
}
