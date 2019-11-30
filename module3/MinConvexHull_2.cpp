#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <limits>
#include <set>



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
	Vector3<T> Projection(size_t coord_plane) const { 
		Vector3<T> result = *this;
		if (coord_plane == 0) // OXY plane
			result.c = 0;
		if (coord_plane == 1) // OYZ plane
			result.a = 0;
		if (coord_plane == 2) // OXZ plane
			result.b = 0;
	}

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
bool operator==(const Vector3<T>& first, const Vector3<T>& second) {
	return IsDoubleEqual(first.a, second.a)
		&& IsDoubleEqual(first.b, second.b)
		&& IsDoubleEqual(first.c, second.c);
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
struct Face {
	Face(const std::vector<Vector3<T>>* all_points) : all_points(all_points) {
		points_i[0] = 0;
		points_i[1] = 1;
		points_i[2] = 2;
	}
	Face(const std::vector<Vector3<T>>* all_points, size_t p_first, size_t p_second, size_t p_third) 
		: all_points(all_points) {
		points_i[0] = p_first;
		points_i[1] = p_second;
		points_i[2] = p_third;
	}
	Face(const std::vector<Vector3<T>>* all_points, const std::array<size_t, 3>& arr)
		: all_points(all_points) { points_i = arr; }

	Vector3<T> NormalVector() const { 
		Vector3<T> vec_prod = Vector3Product(GetPoint(1) - GetPoint(0), GetPoint(2) - GetPoint(0));
		return vec_prod * (1. / vec_prod.Abs());
	}
	Face MinAngleAdjacentFace(size_t p_of_edge, size_t next_p_of_edge) const;
	Face MaxAngleAdjacentFace(size_t p_of_edge, size_t next_p_of_edge) const;
	// move all points in array to get combination when start point is minimal 
	size_t NormalizePoints() {
		size_t min_i = 0;
		for (size_t i = 1; i < points_i.size(); ++i) {
			if (points_i[i] < points_i[min_i])
				min_i = i;
		}
		for (size_t i = 0; i < min_i; ++i) {
			size_t tmp = points_i[0];
			points_i[0] = points_i[1];
			points_i[1] = points_i[2];
			points_i[2] = tmp;
		}
		// returns offset
		return min_i;
	}

	const Vector3<T>& GetPoint(size_t index_in_face) const { return (*all_points)[points_i[index_in_face]]; }
	const std::vector<Vector3<T>>* all_points;
	std::array<size_t, 3> points_i; // indexes of points in std::vector of all points
};

template <class T>
constexpr T k_min_cos = -1;

template <class T>
Face<T> Face<T>::MinAngleAdjacentFace(size_t p_of_edge, size_t next_p_of_edge) const {
	// next_p_of edge must be next after p_of_edge in source face
	Vector3<T> norm_vec = NormalVector();
	Face<T> new_face(all_points);
	// we need to support correct normal vector orientation in new face
	// so we reverse common edge and save it in new face
	new_face.points_i[1] = p_of_edge;
	new_face.points_i[0] = next_p_of_edge;
	// min angle corresponds max cos
	T max_cos = k_min_cos<T>;
	size_t max_cos_point = 0;
	for (size_t i = 0; i < all_points->size(); ++i) {
		if (points_i[0] == i
			|| points_i[1] == i
			|| points_i[2] == i)
			continue;
		// put curr point as last point in new face
		new_face.points_i[2] = i;
		T curr_cos = Scalar3Product(norm_vec, new_face.NormalVector()) 
					/ norm_vec.Abs() * (new_face.NormalVector()).Abs();
		if (curr_cos > max_cos) {
			max_cos = curr_cos;
			max_cos_point = i;
		}
	}
	new_face.points_i[2] = max_cos_point;
	return new_face;
}

template <class T>
constexpr T k_max_cos = 1;

template <class T>
Face<T> Face<T>::MaxAngleAdjacentFace(size_t p_of_edge, size_t next_p_of_edge) const {
	// next_p_of edge must be next after p_of_edge in source face
	Vector3<T> norm_vec = NormalVector();
	Face<T> new_face(all_points);
	// we need to support correct normal vector orientation in new face
	// so we reverse common edge and save it in new face
	new_face.points_i[1] = p_of_edge;
	new_face.points_i[0] = next_p_of_edge;
	// max angle corresponds min cos
	T min_cos = k_max_cos<T>;
	size_t min_cos_point = 0;
	for (size_t i = 0; i < all_points->size(); ++i) {
		if (points_i[0] == i
			|| points_i[1] == i
			|| points_i[2] == i)
			continue;
		// put curr point as last point in new face
		new_face.points_i[2] = i;
		T curr_cos = Scalar3Product(norm_vec, new_face.NormalVector())
			/ norm_vec.Abs() * (new_face.NormalVector()).Abs();
		if (curr_cos < min_cos) {
			min_cos = curr_cos;
			min_cos_point = i;
		}
	}
	new_face.points_i[2] = min_cos_point;
	return new_face;
}

template <class T>
class FaceCmp {
public:
	bool operator()(const Face<T>& first, const Face<T>& second) const {
		// std::array compares lexicographically
		return first.points_i < second.points_i;
	}
};

template <class T>
size_t LowestPoint(const std::vector<Vector3<T>>& all_points) {
	size_t min_p = 0; // min point index
	for (size_t i = 1; i < all_points.size(); ++i) {
		if (all_points[i].c < all_points[min_p].c)
			min_p = i;
		else if (all_points[i].c == all_points[min_p].c
				&& all_points[i].b < all_points[min_p].b)
			min_p = i;
		else if (all_points[i].c == all_points[min_p].c
				&& all_points[i].b == all_points[min_p].b
				&& all_points[i].a < all_points[min_p].a)
			min_p = i;
	}
	return min_p;
}

template <class T>
Face<T> LowestFace(const std::vector<Vector3<T>>& all_points) {
	size_t first_p = LowestPoint(all_points);
	// create horizontal face that passes throw first_p
	// the order of it`s points corresponds to down-directed normal vector
	std::vector<Vector3<T>> all_points_with_adding = all_points;
	all_points_with_adding.push_back(all_points[first_p] + Vector3<T>(0, 1, 0));
	all_points_with_adding.push_back(all_points[first_p] + Vector3<T>(1, 0, 0));
	Face<T> first_face(&all_points_with_adding
					, first_p
					, all_points_with_adding.size() - 2
					, all_points_with_adding.size() - 1);
	// find another face incident to edge (points_i[2], points_i[0])
	Face<T> second_face = first_face.MaxAngleAdjacentFace(first_face.points_i[0], first_face.points_i[1]);
	// definitely there is no points_i[2] point in second face (we found new face different from source face)
	// so we need to delete it from all_points, because this point is additional
	all_points_with_adding.pop_back();
	Face<T> third_face = second_face.MaxAngleAdjacentFace(second_face.points_i[1], second_face.points_i[2]);
	// analogically delete last additional point and get equal to source all_points array
	third_face.all_points = &all_points;
	return third_face;
}

template <class T>
struct Edge {
	Edge(typename std::set<Face<T>, FaceCmp<T>>::iterator face
			, size_t first
			, size_t second) : face(face), first(first), second(second) { }

	void NormalizeEdge(size_t offset) {
		first = (first + face->points_i.size() - offset) % face->points_i.size();
		second = (second + face->points_i.size() - offset) % face->points_i.size();
	}

	// incident face
	typename std::set<Face<T>, FaceCmp<T>>::iterator face;
	// indexes of endings (in corresponding to face order) in face
	size_t first;
	size_t second;
};

template<class T>
void PrintFaces(const std::set<Face<T>, FaceCmp<T>>& faces) {
	std::cout << faces.size() << std::endl;
	for (const auto& face : faces) {
		std::cout << face.points_i.size() << ' ' << face.points_i[0]
			<< ' ' << face.points_i[1] << ' ' << face.points_i[2] << std::endl;
	}
}

template <class T>
void PrintMinConvexHull(const std::vector<Vector3<T>>& all_points) {
	std::set<Face<T>, FaceCmp<T>> faces;
	std::vector<Edge<T>> edges;
	// we need pointer on incedent face to not to search it each moment
	Face<T> curr_face = LowestFace(all_points);
	curr_face.NormalizePoints();
	typename std::set<Face<T>, FaceCmp<T>>::iterator first_it = faces.insert(curr_face).first;
	edges.emplace_back(first_it, 0, 1);
	edges.emplace_back(first_it, 1, 2);
	edges.emplace_back(first_it, 2, 0);

	while (!edges.empty()) {
		Edge<T> edge = edges.back();
		edges.pop_back();

		Face<T> new_face = edge.face->MinAngleAdjacentFace(edge.face->points_i[edge.first]
														, edge.face->points_i[edge.second]);
		// edge points in new face
		size_t first_p_in_edge = new_face.points_i[0];
		size_t second_p_in_edge = new_face.points_i[1];
		// change start point in face to min
		new_face.NormalizePoints();

		if (!faces.count(new_face)) {
			// определить, какую вершину из грани не содержит ребро
			// положить в стек инцидентные ей рёбра
			size_t i = 0;
			// find point that doesn`t contains in common edge
			for (; new_face.points_i[i] == first_p_in_edge
				|| new_face.points_i[i] == second_p_in_edge; ++i);
			
			typename std::set<Face<T>, FaceCmp<T>>::iterator new_it = faces.insert(new_face).first;
			size_t next = (i + 1) % new_face.points_i.size();
			size_t prev = (i - 1 + new_face.points_i.size()) % new_face.points_i.size();
			edges.emplace_back(new_it, i, next);
			edges.emplace_back(new_it, prev, i);
		}
	}
	PrintFaces(faces);
}

int main() {
	size_t tests_cnt = 0;
	std::cin >> tests_cnt;
	for (size_t i = 0; i < tests_cnt; ++i) {
		size_t points_cnt = 0;
		std::cin >> points_cnt;
		std::vector<Vector3<double>> all_points(points_cnt);
		for (size_t i = 0; i < all_points.size(); ++i) {
			std::cin >> all_points[i].a >> all_points[i].b >> all_points[i].c;
		}
		PrintMinConvexHull(all_points);
	}
	return 0;
}
