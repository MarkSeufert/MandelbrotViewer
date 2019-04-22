///Create a class that handles complex number related math
class ComplexNumber
{
public:
	double mReal; //The real component of the complex number
	double mImaginary; //The imaginary component of the complex number
	ComplexNumber(double = 0, double = 0); //The constructor which takes the real and imaginary as parameters
	ComplexNumber operator+(ComplexNumber); //Overload the '+' sign to add complex numbers
	ComplexNumber operator*(ComplexNumber); //Overload the '*' sign to multiply complex numbers
	double SquareMagnitude(); //Returns the squared magnitude of this complex number
};