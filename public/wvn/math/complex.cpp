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

	for (double n = 0.0; n < iter; n += 1.0) {
		result += Complex(1.0) / Complex::pow(n + 1.0, s);
	}

	return result;
}

Complex Complex::gamma(const Complex& z)
{
	return fact(z - Complex(1.0));
}

Complex Complex::fact(const Complex& z)
{
	const double LEN = CalcD::sqrt((z.real * z.real) + (z.imag * z.imag));
	const double DIST = 5.0 + ((LEN * LEN) / 2.0); // i just find that adding 4 gets good performance and good results as that is
	constexpr double DX = 0.01;

	Complex result = Complex::zero();

	for (double x = 0.0; x < DIST; x += DX) {
		result += Complex::pow(Complex(x), z) * Complex::exp(Complex(-x));
	}

	return result;
}

Complex Complex::pow(const Complex& z, const Complex& p)
{
	if (z == Complex::zero())
		return Complex::zero();

	Complex real_power = z;
	{
		double c1 = CalcD::floor(p.real);
		double c2 = p.real - c1;

		for (double i = 1.0; i < c1; i += 1.0) {
			real_power *= z;
		}

		real_power *= Complex::root(z, Complex(c2));
	}

	Complex imag_power =
		Complex::cos(Complex::ln(z) * p.imag) +
		(Complex(0.0, 1.0) * Complex::sin(Complex::ln(z) * p.imag));

	return real_power * imag_power;
}

Complex Complex::exp(const wvn::Complex& b)
{
	return pow(b, Complex(math::E));
}

Complex Complex::log(const wvn::Complex& z, const wvn::Complex& b)
{
	return Complex::ln(z) / Complex::ln(b);
}

Complex Complex::ln(const Complex& z)
{
	// ln(z) = ln(radius*e^(i*theta)) = ln(radius) + theta*i

	return Complex(
		CalcD::ln(CalcD::sqrt((z.real * z.real) + (z.imag * z.imag))),
		CalcD::atan2(z.imag, z.real)
	);
}

Complex Complex::sqrt(const Complex& z)
{
	return Complex::root(z, Complex(2.0, 0.0));
//	return Complex(
//		CalcD::sqrt(z.real + ((z.imag * z.imag) / ((4.0 * z.real) + 1))),
//		z.imag / CalcD::sqrt(z.real + 1.0)
//	);
}

Complex Complex::cbrt(const Complex& z)
{
	return Complex::root(z, Complex(3.0, 0.0));
}

Complex Complex::root(const Complex& z, const wvn::Complex& b)
{
	return Complex::pow(z, Complex(1.0) / b);
}

Complex Complex::cos(const Complex& z)
{
	return Complex(
		CalcD::cos(z.real) * CalcD::cosh(z.imag),
		-CalcD::sin(z.real) * CalcD::sinh(z.imag)
	);
}

Complex Complex::sin(const Complex& z)
{
	return Complex(
		CalcD::sin(z.real) * CalcD::cosh(z.imag),
		CalcD::cos(z.real) * CalcD::sinh(z.imag)
	);
}

Complex Complex::tan(const Complex& z)
{
	return sin(z) / cos(z);
}

Complex Complex::cot(const Complex& z)
{
	return cos(z) / sin(z);
}

Complex Complex::sec(const Complex& z)
{
	return Complex(1.0) / cos(z);
}

Complex Complex::csc(const Complex& z)
{
	return Complex(1.0) / sin(z);
}

Complex Complex::acos(const Complex& z)
{
}

Complex Complex::asin(const Complex& z)
{
}

Complex Complex::atan(const Complex& z)
{
}

Complex Complex::acot(const Complex& z)
{
}

Complex Complex::asec(const Complex& z)
{
}

Complex Complex::acsc(const Complex& z)
{
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
	(*this) = (*this) + other;
	return *this;
}

Complex& Complex::operator -= (const Complex& other)
{
	(*this) = (*this) - other;
	return *this;
}

Complex& Complex::operator *= (const Complex& other)
{
	(*this) = (*this) * other;
	return *this;
}

Complex& Complex::operator /= (const Complex& other)
{
	(*this) = (*this) / other;
	return *this;
}

const Complex& Complex::zero() { static const Complex ZERO = Complex(0.0, 0.0); return ZERO; }
const Complex& Complex::one()  { static const Complex ONE  = Complex(1.0, 1.0); return ONE;  }
