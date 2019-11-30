#include <iostream>
#include <stdint.h>
#include <vector>
#include <cmath>
#include <limits.h>
#include <iomanip>



constexpr double k_accuracy_com = 0.000000000001;
constexpr double min_subsegment_length = 0.000000000001;

template <class T>
bool IsDoubleEqual(T first, T second, const T k_accuracy = k_accuracy_com) {
	bool result = false;
	if (abs(first - second) < k_accuracy)
		result = true;
	return result;
}

template <class T>
bool IsDoubleLess(T first, T second, const T k_accuracy = k_accuracy_com) {
	bool result = false;
	if (first + k_accuracy < second)
		result = true;
	return result;
}

template <class T>
bool IsDoubleGreater(T first, T second, const T k_accuracy = k_accuracy_com) {
	bool result = false;
	if (first > second + k_accuracy)
		result = true;
	return result;
}

template <class T>
struct Vector {
	Vector(T a, T b, T c) : a(a), b(b), c(c) { }

	T Abs() const { return sqrt(a * a + b * b + c * c); }

	Vector<T> operator-() const { return Vector<T>(-a, -b, -c); }
	Vector<T>& operator+=(const Vector<T>& v_other) & {
		a += v_other.a;
		b += v_other.b;
		c += v_other.c;
		return *this;
	}
	Vector<T> operator-=(const Vector<T>& v_other) & {
		a -= v_other.a;
		b -= v_other.b;
		c -= v_other.c;
		return *this;
	}
	Vector<T> operator*=(const T& coeff) & {
		a *= coeff;
		b *= coeff;
		c *= coeff;
		return *this;
	}
	operator bool() const {
		return IsDoubleEqual(a, 0) && IsDoubleEqual(b, 0) && IsDoubleEqual(c, 0) ? false : true;
	}

	T a;
	T b;
	T c;
};

template <class T>
Vector<T> operator+(const Vector<T>& v_first, const Vector<T>& v_second) {
	Vector<T> result = v_first;
	result += v_second;
	return result;
}

template <class T>
Vector<T> operator-(const Vector<T>& v_first, const Vector<T>& v_second) {
	Vector<T> result = v_first;
	result -= v_second;
	return result;
}

template <class T>
Vector<T> operator*(const Vector<T>& v, const T& coeff) {
	Vector<T> result = v;
	result *= coeff;
	return result;
}

template <class T>
T PointsDistance(const Vector<T>& a, const Vector<T>& b) {
	return (b - a).Abs();
}

template <class T>
struct LineSegment {
	LineSegment(Vector<T> p_left, Vector<T> p_right) : p_left(p_left), p_right(p_right) { }
	T Length() const { return (p_right - p_left).Abs(); }

	Vector<T> p_left;
	Vector<T> p_right;
};

template <class T, class ObjectT>
class UnimodFunc;

template <class T, class ObjectT>
T TernarySearch(const LineSegment<T>& seg // where we search min value
	, UnimodFunc<T, ObjectT> unimod_func); // line_segment or point

template <class T, class ObjectT>
class UnimodFunc {
public:
	UnimodFunc(const ObjectT& object) : object(object) { }
	T GetValue(const Vector<T>& point) const {
		return Distance(point, object);
	}
private:
	ObjectT object;
	T Distance(const Vector<T>& point, const LineSegment<T>& seg) const {
		return TernarySearch(seg, UnimodFunc<T, Vector<T>>(point));
	}
	T Distance(const Vector<T>& point, const Vector<T>& other_point) const {
		return PointsDistance(point, other_point);
	}
};

template <class T, class ObjectT>
T TernarySearch(const LineSegment<T>& seg // where we search min value
				, UnimodFunc<T, ObjectT> unimod_func) { // line_segment or point
	Vector<T> left_end = seg.p_left;
	Vector<T> right_end = seg.p_right;
	Vector<T> segm_p_left(0, 0, 0); // new endings of current searching line segment
	Vector<T> segm_p_right(0, 0, 0);

	// checking line_segment`s length to stop if 
	// required accuracy is reached
	while (IsDoubleGreater(PointsDistance(left_end, right_end), min_subsegment_length)) {
		// new endings
		segm_p_left = left_end + (right_end - left_end) * (1. / 3);
		segm_p_right = left_end + (right_end - left_end) * (2. / 3);

		if (!IsDoubleGreater(unimod_func.GetValue(segm_p_left)
			, unimod_func.GetValue(segm_p_right)))
			right_end = segm_p_right;
		if (!IsDoubleLess(unimod_func.GetValue(segm_p_left)
			, unimod_func.GetValue(segm_p_right))) {
			left_end = segm_p_left;
		}
	}
	return unimod_func.GetValue(left_end);
}

int main() {
	double x = 0;
	double y = 0;
	double z = 0;

	std::cin >> x >> y >> z;
	Vector<double> left(x, y, z);
	std::cin >> x >> y >> z;
	Vector<double> right(x, y, z);
	LineSegment<double> first(left, right);

	std::cin >> x >> y >> z;
	left = Vector<double>(x, y, z);
	std::cin >> x >> y >> z;
	right = Vector<double>(x, y, z);
	LineSegment<double> second(left, right);

	std::cout << std::setprecision(20) << TernarySearch(first, 
											UnimodFunc<double, LineSegment<double>>(second));
	return 0;
}