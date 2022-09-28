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
	const double DIST = 10.0 + (z.magnitude_squared() / 2.0);
	constexpr double DX = 0.01;

	Complex result = Complex::zero();

	for (double x = 0.0; x < DIST; x += DX) {
		result += (Complex::pow(Complex(x, 0.0), z) / CalcD::exp(x)) * DX;
	}

	return result;
}

Complex Complex::pow(const Complex& z, const Complex& p)
{
	if (z == Complex::zero())
		return Complex::zero();

	if (p == Complex::zero())
		return Complex::one();

	double r = z.magnitude();
	double theta = z.argument();
	double lnr = CalcD::ln(r);
	double exp0 = CalcD::exp(lnr   *  p.real);
	double exp1 = CalcD::exp(theta * -p.imag);

	Complex part0 = Complex(exp0) *
		((Complex::one() * Complex::cos(lnr * p.imag)) +
		 (Complex::i  () * Complex::sin(lnr * p.imag)));

	Complex part1 = Complex(exp1) *
		((Complex::one() * Complex::cos(theta * p.real)) +
		 (Complex::i  () * Complex::sin(theta * p.real)));

	return part0 * part1;
}

Complex Complex::exp(const wvn::Complex& b)
{
	return pow(b, Complex(CalcD::E));
}

Complex Complex::log(const wvn::Complex& z, const wvn::Complex& b)
{
	return Complex::ln(z) / Complex::ln(b);
}

Complex Complex::log10(const wvn::Complex& z)
{
	return Complex::ln(z) / Complex(2.302585093);
}

Complex Complex::log2(const wvn::Complex& z)
{
	return Complex::ln(z) / Complex(0.693147180559945);
}

Complex Complex::ln(const Complex& z)
{
	return Complex(
		CalcD::ln(z.magnitude()),
		z.argument()
	);
}

Complex Complex::sqrt(const Complex& z)
{
	return Complex::root(z, Complex(2.0));
}

Complex Complex::cbrt(const Complex& z)
{
	return Complex::root(z, Complex(3.0));
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

// i dont know what to put for the complex number definitions

Complex Complex::acos(const Complex& z)
{
	return CalcD::acos(z.real);
}

Complex Complex::asin(const Complex& z)
{
	return CalcD::asin(z.real);
}

Complex Complex::atan(const Complex& z)
{
	return CalcD::atan(z.real);
}

constexpr double Complex::magnitude() const
{
	return CalcD::sqrt((real * real) + (imag * imag));
}

constexpr double Complex::magnitude_squared() const
{
	return (real * real) + (imag * imag);
}

constexpr double Complex::argument() const
{
	return CalcD::atan2(imag, real);
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
const Complex& Complex::one()  { static const Complex ONE  = Complex(1.0, 0.0); return ONE;  }
const Complex& Complex::i()    { static const Complex I    = Complex(0.0, 1.0); return I;    }
