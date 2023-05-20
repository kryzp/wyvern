#ifndef CALC_H
#define CALC_H

#include <limits>
#include <cmath>

namespace wvn
{
	/**
	 * Generic calculator-wrapper for maths.
	 */
	template <typename T>
	class Calc
	{
	public:
		constexpr static T E       = 2.71828182845;
		constexpr static T PI      = 3.14159265359;
		constexpr static T TAU     = 6.28318530718;
		constexpr static T RAD2DEG = 57.2957795131;
		constexpr static T DEG2RAD = 0.01745329251;

		static T abs(T x);
		static T mod(T x, T y);
		static T sqrt(T x);

		static T pow(T x, T e);
		static T exp(T x);
		static T sigmoid(T x);
		static T sign(T x);
		static T snap(T x, T interval);

		static T log(T x, T b);
		static T log2(T x);
		static T log10(T x);
		static T ln(T x);

		static T max(T x, T y);
		static T min(T x, T y);
		static T clamp(T x, T mn, T mx);

		static T round(T x);
		static T floor(T x);
		static T ceil(T x);
		static T fract(T x);

		static bool within_epsilon(T lhs, T rhs, T epsilon = std::numeric_limits<T>::epsilon());

		static T approach(T from, T to, T amount);
		static T lerp(T from, T to, T t);
		static T smooth(T from, T to, T amount, T time);
		static T spring(T from, T to, T bounciness, T tension, T& intermediate);
		static T spring_t(T from, T to, T bounciness, T tension, T time);

		static T sin(T x);
		static T cos(T x);
		static T tan(T x);

		static T sinh(T x);
		static T cosh(T x);
		static T tanh(T x);

		static T asin(T x);
		static T acos(T x);
		static T atan(T x);
		static T atan2(T y, T x);

		static T epsilon();
		static T lowest();
		static T min_value();
		static T max_value();
		static T infinity();
	};

	using CalcI = Calc<int>;
	using CalcU = Calc<unsigned>;
	using CalcF = Calc<float>;
	using CalcD = Calc<double>;

	template <typename T>
	T Calc<T>::abs(T x)
	{
		return x > 0.0 ? x : -x;
	}

	template <typename T>
	T Calc<T>::mod(T x, T y)
	{
		return std::fmodf(x, y);
	}

	template <typename T>
	T Calc<T>::sqrt(T x)
	{
		return std::sqrt(x);
	}

	template <typename T>
	T Calc<T>::pow(T x, T e)
	{
		return std::pow(x, e);
	}

	template <typename T>
	T Calc<T>::exp(T x)
	{
		return std::exp(x);
	}

	template <typename T>
	T Calc<T>::sigmoid(T x)
	{
		return 1.0 - (1.0 / (1.0 + std::exp(x)));
	}

	template <typename T>
	T Calc<T>::sign(T x)
	{
		return (x < 0.0) ? -1.0 : (x > 0.0) ? 1.0 : 0.0;
	}

	template <typename T>
	T Calc<T>::snap(T x, T interval)
	{
		if (interval <= 1.0) {
			return std::floor(x) + std::round(x - std::floor(x));
		} else {
			return std::round(x / interval) * interval;
		}
	}

	template <typename T>
	T Calc<T>::log(T x, T b)
	{
		return std::log(x) / std::log(b);
	}

	template <typename T>
	T Calc<T>::log2(T x)
	{
		return std::log2(x);
	}

	template <typename T>
	T Calc<T>::log10(T x)
	{
		return std::log10(x);
	}

	template <typename T>
	T Calc<T>::ln(T x)
	{
		return std::log(x);
	}

	template <typename T>
	T Calc<T>::max(T x, T y)
	{
		return (x > y) ? x : y;
	}

	template <typename T>
	T Calc<T>::min(T x, T y)
	{
		return (x < y) ? x : y;
	}

	template <typename T>
	T Calc<T>::clamp(T x, T mn, T mx)
	{
		return min(mx, max(mn, x));
	}

	template <typename T>
	T Calc<T>::round(T x)
	{
		return std::round(x);
	}

	template <typename T>
	T Calc<T>::floor(T x)
	{
		return std::floor(x);
	}

	template <typename T>
	T Calc<T>::ceil(T x)
	{
		return std::ceil(x);
	}

	template <typename T>
	T Calc<T>::fract(T x)
	{
		return x - std::floor(x);
	}
	
	template <typename T>
	bool Calc<T>::within_epsilon(T lhs, T rhs, T epsilon)
	{
		return std::fabsf(rhs - lhs) <= epsilon;
	}

	template <typename T>
	T Calc<T>::approach(T from, T to, T amount)
	{
		return (to > from) ? min(from + amount, to) : max(from - amount, to);
	}

	template <typename T>
	T Calc<T>::lerp(T from, T to, T t)
	{
		return from + ((to - from) * t);
	}

	template <typename T>
	T Calc<T>::smooth(T from, T to, T amount, T time)
	{
		return (std::exp(amount * time / (amount - 1.0)) * (from - to)) + to;
	}

	template <typename T>
	T Calc<T>::spring(T from, T to, T bounciness, T tension, T& intermediate)
	{
		intermediate = lerp(intermediate, (to - from) * tension, 1.0 / bounciness);
		return from + intermediate;
	}

	/*
	 * I'm actually really proud of this because I got down
	 * and actually managed to solve the differential
	 * equation properly over the course of 3 pages! :D
	 */
	template <typename T>
	T Calc<T>::spring_t(T from, T to, T bounciness, T tension, T time)
	{
		T beta = std::sqrt((4.0 * bounciness * tension) - 1.0);
		return to - (to / beta * std::exp(-time / (2.0 * bounciness)) * (std::sin(beta * time / (2.0 * bounciness)) + (beta * std::cos(beta * time / (2.0 * bounciness)))));
	}

	template <typename T>
	T Calc<T>::sin(T x)
	{
		return std::sin(x);
	}

	template <typename T>
	T Calc<T>::cos(T x)
	{
		return std::cos(x);
	}

	template <typename T>
	T Calc<T>::tan(T x)
	{
		return std::tan(x);
	}

	template <typename T>
	T Calc<T>::sinh(T x)
	{
		return std::sinh(x);
	}

	template <typename T>
	T Calc<T>::cosh(T x)
	{
		return std::cosh(x);
	}

	template <typename T>
	T Calc<T>::tanh(T x)
	{
		return std::tanh(x);
	}

	template <typename T>
	T Calc<T>::asin(T x)
	{
		return std::asin(x);
	}

	template <typename T>
	T Calc<T>::acos(T x)
	{
		return std::acos(x);
	}

	template <typename T>
	T Calc<T>::atan(T x)
	{
		return std::atan(x);
	}

	template <typename T>
	T Calc<T>::atan2(T y, T x)
	{
		return std::atan2(y, x);
	}

	template <typename T>
	T Calc<T>::epsilon()
	{
		return std::numeric_limits<T>::epsilon();
	}

	template <typename T>
	T Calc<T>::lowest()
	{
		return std::numeric_limits<T>::lowest();
	}

	template <typename T>
	T Calc<T>::min_value()
	{
		return std::numeric_limits<T>::min();
	}

	template <typename T>
	T Calc<T>::max_value()
	{
		return std::numeric_limits<T>::max();
	}

	template <typename T>
	T Calc<T>::infinity()
	{
		return std::numeric_limits<T>::infinity();
	}
}

#endif // CALC_H
