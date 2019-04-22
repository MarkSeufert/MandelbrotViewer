#include "ComplexNumbers.h"

ComplexNumber::ComplexNumber(double pReal, double pImaginary)
{
	mReal = pReal;
	mImaginary = pImaginary;
}

ComplexNumber ComplexNumber::operator+(ComplexNumber pNum)
{
	return ComplexNumber(mReal + pNum.mReal, mImaginary + pNum.mImaginary);
}

ComplexNumber ComplexNumber::operator*(ComplexNumber pNum)
{
	double real = mReal * pNum.mReal - mImaginary * pNum.mImaginary;
	double imaginary = mReal * pNum.mImaginary + mImaginary * pNum.mReal;
	return ComplexNumber(real, imaginary);
}

double ComplexNumber::SquareMagnitude()
{
	return mReal * mReal + mImaginary * mImaginary;
}
