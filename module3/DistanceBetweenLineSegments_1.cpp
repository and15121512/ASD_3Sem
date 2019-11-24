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

template <class T>
T PointAndSegmentDistance(const Vector<T>& point, const LineSegment<T>& seg) {
	Vector<T> left_end = seg.p_left;
	Vector<T> right_end = seg.p_right;
	Vector<T> segm_p_left(0, 0, 0);
	Vector<T> segm_p_right(0, 0, 0);

	while (IsDoubleGreater(PointsDistance(left_end, right_end), min_subsegment_length)) {
		segm_p_left = left_end + (right_end - left_end) * (1. / 3);
		segm_p_right = left_end + (right_end - left_end) * (2. / 3);

		if (!IsDoubleGreater(PointsDistance(point, segm_p_left)
			, PointsDistance(point, segm_p_right)))
			right_end = segm_p_right;
		if (!IsDoubleLess(PointsDistance(point, segm_p_left)
			, PointsDistance(point, segm_p_right))) {
			left_end = segm_p_left;
		}
	}

	return PointsDistance(left_end, point);
}

template <class T>
T LineSegmentsDistance(const LineSegment<T>& seg_first, const LineSegment<T>& seg_second) {
	// find distance between each point of the first segment and second segment
	// and than return min value
	Vector<T> left_end = seg_first.p_left;
	Vector<T> right_end = seg_first.p_right;
	Vector<T> segm_p_left(0, 0, 0);
	Vector<T> segm_p_right(0, 0, 0);

	while (PointsDistance(left_end, right_end) > min_subsegment_length) {
		segm_p_left = left_end + (right_end - left_end) * (1. / 3);
		segm_p_right = left_end + (right_end - left_end) * (2. / 3);

		if (!IsDoubleGreater(PointAndSegmentDistance(segm_p_left, seg_second)
			, PointAndSegmentDistance(segm_p_right, seg_second))) {
			right_end = segm_p_right;
		}
		if (!IsDoubleLess(PointAndSegmentDistance(segm_p_left, seg_second)
			, PointAndSegmentDistance(segm_p_right, seg_second))) {
			left_end = segm_p_left;
		}
	}

	return PointAndSegmentDistance(left_end, seg_second);
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

	std::cout << std::setprecision(20) << LineSegmentsDistance(first, second);
	return 0;
}
