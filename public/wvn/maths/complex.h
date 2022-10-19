#pragma once

// idk why i made this it serves like negative purpose

namespace wvn
{
	struct Complex
	{
		double real;
		double imag;

		Complex();
		Complex(double real);
		Complex(double real, double imag);
		~Complex() = default;

		static const Complex& zero();
		static const Complex& one();
		static const Complex& i();

		static Complex zeta(const Complex& s, unsigned iter = 48);
		static Complex gamma(const Complex& z);
		static Complex factorial(const Complex& z);

		static Complex pow(const Complex& z, const Complex& p);
		static Complex exp(const Complex& b);

		static Complex log(const Complex& z, const Complex& b);
		static Complex log10(const Complex& z);
		static Complex log2(const Complex& z);
		static Complex ln(const Complex& z);

		static Complex sqrt(const Complex& z);
		static Complex cbrt(const Complex& z);
		static Complex root(const Complex& z, const Complex& b);

		static Complex sin(const Complex& z);
		static Complex cos(const Complex& z);
		static Complex tan(const Complex& z);

		static Complex asin(const Complex& z);
		static Complex acos(const Complex& z);
		static Complex atan(const Complex& z);

		constexpr double magnitude() const;
		constexpr double magnitude_squared() const;
		constexpr double argument() const;
		Complex conjugate() const;

		bool operator == (const Complex& other) const;
		bool operator != (const Complex& other) const;

		Complex operator + (const Complex& other) const;
		Complex operator - (const Complex& other) const;
		Complex operator * (const Complex& other) const;
		Complex operator / (const Complex& other) const;

		Complex operator - () const;

		Complex& operator += (const Complex& other);
		Complex& operator -= (const Complex& other);
		Complex& operator *= (const Complex& other);
		Complex& operator /= (const Complex& other);
	};
}
