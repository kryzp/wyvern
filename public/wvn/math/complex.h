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

		static Complex zeta(const Complex& s, unsigned iter = 32);
		static Complex pow(const Complex& base, const Complex& power);
		static Complex ln(const Complex& z); // note: returns the *principal* log[e] (no tau*n)
		static Complex cos(const Complex& value);
		static Complex sin(const Complex& value);

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
