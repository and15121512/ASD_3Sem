#include <iostream>
#include <stdint.h>
#include <vector>
#include <string>
#include <math.h>
#include <limits.h>
#include <iomanip>



bool IsDoubleEqual(double first, double second) {
	const double k_accuracy = 0.000000000000001;
	bool result = false;
	if (abs(first - second) < k_accuracy)
		result = true;
	return result;
}

bool IsDoubleLess(double first, double second) {
	const double k_accuracy = 0.000000000000001;
	bool result = false;
	if (first + k_accuracy < second)
		result = true;
	return result;
}

bool IsDoubleGreater(double first, double second) {
	const double k_accuracy = 0.000000000000001;
	bool result = false;
	if (first > second + k_accuracy)
		result = true;
	return result;
}

template <class T>
struct Vector3 {
	Vector3() : a(0), b(0), c(0) { }
	Vector3(T a, T b) : a(a), b(b), c(0) { }
	Vector3(T a, T b, T c) : a(a), b(b), c(c) { }

	T Abs() const { return sqrt(a * a + b * b + c * c); }

	Vector3<T> operator-() const { return Vector3<T>(-a, -b, -c); }
	Vector3<T>& operator+=(const Vector3<T>& v_other) & {
		a += v_other.a;
		b += v_other.b;
		c += v_other.c;
		return *this;
	}
	Vector3<T> operator-=(const Vector3<T>& v_other) & {
		a -= v_other.a;
		b -= v_other.b;
		c -= v_other.c;
		return *this;
	}
	Vector3<T> operator*=(const T& coeff) & {
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
Vector3<T> operator+(const Vector3<T>& v_first, const Vector3<T>& v_second) {
	Vector3<T> result = v_first;
	result += v_second;
	return result;
}

template <class T>
Vector3<T> operator-(const Vector3<T>& v_first, const Vector3<T>& v_second) {
	Vector3<T> result = v_first;
	result -= v_second;
	return result;
}

template <class T>
Vector3<T> operator*(const Vector3<T>& v, const T& coeff) {
	Vector3<T> result = v;
	result *= coeff;
	return result;
}

template <class T>
T Scalar3Product(const Vector3<T>& v_first, const Vector3<T>& v_second) {
	return v_first.a * v_second.a + v_first.b * v_second.b + v_first.c * v_second.c;
}

template<class T>
Vector3<T> Vector3Product(const Vector3<T>& v_first, const Vector3<T>& v_second) {
	return Vector3<T>(v_first.b * v_second.c - v_first.c * v_second.b
		, v_first.c * v_second.a - v_first.a * v_second.c
		, v_first.a * v_second.b - v_first.b * v_second.a);
}

template <class T>
struct Polygon {
	Polygon() {}
	Polygon(const std::vector<Vector3<T>>& vertices) : vertices(vertices) { }
	void Negation() {
		for (auto& vertex : vertices)
			vertex = -vertex;
	}
	bool IsPointInPolygon(const Vector3<T>& p) const;
	bool IsPointInEdge(size_t i, const Vector3<T>& p) const;
	size_t LowestLeftPoint() const;
	std::vector<Vector3<T>> vertices;
};

template <class T> 
size_t Polygon<T>::LowestLeftPoint() const {
	size_t res_p = 0;
	for (size_t i = 0; i < vertices.size(); ++i) {
		if (IsDoubleLess(vertices[i].b, vertices[res_p].b)) {
			res_p = i;
		}
		else if (IsDoubleEqual(vertices[i].b, vertices[res_p].b) 
				&& IsDoubleLess(vertices[i].a, vertices[res_p].a)) {
			res_p = i;
		}
	}
	return res_p;
}

template <class T>
bool Polygon<T>::IsPointInEdge(size_t i, const Vector3<T>& p) const {
	Vector3<T> first = p - vertices[i];
	Vector3<T> second = p - vertices[(i + 1) % vertices.size()];
	bool result = true;
	if (IsDoubleGreater(Scalar3Product(first, second), 0))
		result = false;
	return result;
}

template <class T>
bool Polygon<T>::IsPointInPolygon(const Vector3<T>& p) const {
	bool result = true;
	for (size_t i = 0; i < vertices.size(); ++i) {
		Vector3<T> edge = vertices[(i + 1) % vertices.size()] - vertices[i];
		if (!IsDoubleLess((Vector3Product<T>(edge, p - vertices[i])).c, 0)) {
			result = IsDoubleEqual((Vector3Product<T>(edge, p - vertices[i])).c, 0) 
								&& IsPointInEdge(i, p) ? true : false;
			break;
		}
	}
	return result;
}

template <class T>
bool IsAngleLess(const Polygon<T>& pol_first, int32_t first_vertex
	, const Polygon<T>& pol_second, int32_t second_vertex) {
	const Vector3<T> horiz_vector = Vector3<T>(-1, 0, 0);

	Vector3<T> first_v = pol_first.vertices[first_vertex];
	Vector3<T> first_next_v = pol_first.vertices[(first_vertex + 1) % pol_first.vertices.size()];
	Vector3<T> first = first_next_v - first_v;
	Vector3<T> second_v = pol_second.vertices[second_vertex];
	Vector3<T> second_next_v = pol_second.vertices[(second_vertex + 1) % pol_second.vertices.size()];
	Vector3<T> second = second_next_v - second_v;

	T first_value = Scalar3Product(first, horiz_vector)
		/ (first.Abs() * horiz_vector.Abs());
	T second_value = Scalar3Product(second, horiz_vector)
		/ (second.Abs() * horiz_vector.Abs());

	bool result = false;
	if (IsDoubleGreater((Vector3Product<T>(first, horiz_vector)).c, 0)) {
		// 0 < angle_first < pi
		if (IsDoubleGreater((Vector3Product<T>(second, horiz_vector)).c, 0)) {
			// 0 < angle_second < pi
			result = IsDoubleGreater(first_value, second_value);
		}
		else {
			result = true;
		}
	}
	else {
		// pi < angle_first < 2 * pi
		if (IsDoubleGreater((Vector3Product<T>(second, horiz_vector)).c, 0)) {
			// 0 < angle_second < pi
			result = false;
		}
		else {
			result = IsDoubleLess(first_value, second_value);
		}
	}

	return result;
}

template<class T>
Polygon<T> MinkovskiyAddition(const Polygon<T>& pol_first, const Polygon<T>& pol_second) {
	size_t diff_first = pol_first.LowestLeftPoint();
	size_t diff_second = pol_second.LowestLeftPoint();
	std::vector<Vector3<T>> result_vertices;
	size_t i = 0;
	size_t j = 0;
	while (i < pol_first.vertices.size() || j < pol_second.vertices.size()) {
		size_t real_i = (i + diff_first) % pol_first.vertices.size();
		size_t real_j = (j + diff_second) % pol_second.vertices.size();
		result_vertices.push_back(pol_first.vertices[real_i] + pol_second.vertices[real_j]);
		// in clear algorithm we needn`t to use first part of conditions below, 
		// but according to implementation of angle calculating (angle must be in [0, 2 * pi])
		// it is neccessary to check this cases
		if (i < pol_first.vertices.size() && 
			(j >= pol_second.vertices.size() || IsAngleLess<T>(pol_first, real_i, pol_second, real_j)))
			++i;
		else if (j < pol_second.vertices.size() && 
			(i >= pol_first.vertices.size() || IsAngleLess<T>(pol_second, real_j, pol_first, real_i)))
			++j;
		else {
			++i; ++j;
		}
	}
	return Polygon<T>(result_vertices);
}

template <class T>
bool ArePolygonsIntersect(const Polygon<T>& pol_first, const Polygon<T>& pol_second) {
	Polygon<T> negation(pol_second);
	negation.Negation();
	Polygon<T> minkovskiy_addition(MinkovskiyAddition(pol_first, negation));
	return minkovskiy_addition.IsPointInPolygon(Vector3<T>(0, 0, 0));
}

int main() {
	size_t first_sz = 0;
	std::cin >> first_sz;
	std::vector<Vector3<double>> vec(first_sz);
	for (auto& elem : vec) {
		std::cin >> elem.a >> elem.b;
		elem.c = 0;
	}
	Polygon<double> pol_1(vec);
	
	vec.clear();
	size_t second_sz = 0;
	std::cin >> second_sz;
	vec.resize(second_sz);
	for (auto& elem : vec) {
		std::cin >> elem.a >> elem.b;
		elem.c = 0;
	}
	Polygon<double> pol_2(vec);
	std::string result = ArePolygonsIntersect(pol_1, pol_2) ? "YES" : "NO";
	std::cout << result << std::endl;

	return 0;
}
