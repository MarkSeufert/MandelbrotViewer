class MandelbrotSet {
private:
	//Private variables
	double mWindowWidth, mWindowHeight; //The width and height of the window in pixels
	double mCenterReal, mCenterImaginary; //The center of the screen in (real,imaginary) coordinates
	double** mMandelbrotMemory = NULL; //The memory to store the mandelbrot set. The array is of size mScreenWidth * mScreenHeight
	double mScale; //The range of values on the real axis
	double mIterationMax; //The max number of iterations we will use for the mandelbrot algorithm

	//Private methods
	double MandelbrotAlgorithm(double, double); //The mandelbrot algorithm applied at this point
	void CalculateMandelbrotSet(); //Calculate the mandelbrot set across all the points and save it into mMandelbrotMemory
	void ScreenToComplexPosition(double, double, double&, double&); //Given a position on the screen in pixels, return by reference the real and imaginary associated with it
	void ColoringAlgorithm(double, double&, double&, double&); //Takes the iteration number as an input (0 to 1) and 3 rgb values as reference
public:
	//Public methods
	MandelbrotSet(); //The constructor which gives initial values to several variables
	void Reset(); //Reset the mandelbrot set with initial variables
	void SetScreenSize(int, int); //Call this when the screen size is changed, pass the new screenWidth and screenHeight
	void ZoomIn(double, double, double, double); //Zoom into the set given 2 sets of x,y mouse positions
	void ZoomOut(double); //Zoom out by some amount
	void Draw(); //Draw the mandelbrot set
	void DrawSelectBox(double, double, double, double); //Draw a box from a starting point to an ending point
};