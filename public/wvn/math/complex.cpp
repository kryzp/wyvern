#include <wvn/math/complex.h>
#include <wvn/math/calc.h>

using namespace wvn;

Complex::Complex()
	: real(0.0)
	, imag(0.0)
{
}

Complex::Complex(double real)
	: real(real)
	, imag(0.0)
{
}

Complex::Complex(double real, double imag)
	: real(real)
	, imag(imag)
{
}

Complex Complex::zeta(const Complex& s, unsigned iter)
{
	Complex result = Complex::zero();

	for (double n = 0.0; n < iter; n += 1.0)
		result += Complex(1.0) / (Complex::pow(n + 1.0, s));

	return result;
}

Complex Complex::pow(const Complex& base, const Complex& power)
{
	Complex real_power = base;
	{
		for (double i = 1.0; i < power.real; i += 1.0)
			real_power *= base;
	}

	Complex imag_power =
		Complex::cos(Complex::ln(base) * power.imag) +
		(Complex(0.0, 1.0) * Complex::sin(Complex::ln(base) * power.imag));

	return real_power * imag_power;
}

Complex Complex::ln(const Complex& z)
{
	// ln(z) = ln(radius*e^(i*theta)) = ln(radius) + theta*i

	return Complex(
		CalcD::ln(::sqrt((z.real * z.real) + (z.imag * z.imag))),
		CalcD::atan2(z.imag, z.real)
	);
}

Complex Complex::cos(const Complex& value)
{
	return Complex(
		CalcD::cos(value.real) * CalcD::cosh(value.imag),
		-CalcD::sin(value.real) * CalcD::sinh(value.imag)
	);
}

Complex Complex::sin(const Complex& value)
{
	return Complex(
		CalcD::sin(value.real) * CalcD::cosh(value.imag),
		CalcD::cos(value.real) * CalcD::sinh(value.imag)
	);
}

Complex Complex::conjugate() const
{
	return Complex(real, -imag);
}

bool Complex::operator == (const Complex& other) const
{
	return (this->real == other.real) && (this->imag == other.imag);
}

bool Complex::operator != (const Complex& other) const
{
	return !(*this == other);
}

Complex Complex::operator + (const Complex& other) const
{
	return Complex(this->real + other.real, this->imag + other.imag);
}

Complex Complex::operator - (const Complex& other) const
{
	return Complex(this->real - other.real, this->imag - other.imag);
}

Complex Complex::operator * (const Complex& other) const
{
	return Complex(
		(this->real * other.real) - (this->imag * other.imag),
		(this->real * other.imag) + (this->imag * other.real)
	);
}

Complex Complex::operator / (const Complex& other) const
{
	double divisor = (other.real * other.real) + (other.imag * other.imag);

	return Complex(
		((this->real * other.real) + (this->imag * other.imag)) / divisor,
		((this->imag * other.real) - (this->real * other.imag)) / divisor
	);
}

Complex Complex::operator - () const
{
	return Complex(-real, -imag);
}

Complex& Complex::operator += (const Complex& other)
{
	(*this) = *this + other;
	return *this;
}

Complex& Complex::operator -= (const Complex& other)
{
	(*this) = *this - other;
	return *this;
}

Complex& Complex::operator *= (const Complex& other)
{
	(*this) = *this * other;
	return *this;
}

Complex& Complex::operator /= (const Complex& other)
{
	(*this) = *this / other;
	return *this;
}

const Complex& Complex::zero() { static const Complex ZERO = Complex(0.0, 0.0); return ZERO; }
const Complex& Complex::one()  { static const Complex ONE  = Complex(1.0, 1.0); return ONE;  }
