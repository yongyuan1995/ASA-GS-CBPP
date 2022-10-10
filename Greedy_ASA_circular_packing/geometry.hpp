#pragma once
#include <vector>
#include <set>
#include <cmath>
#include <functional>
#include <queue>
#include <algorithm>

using namespace std;

struct Point {
	double x, y;
	//Point(){}
	Point(double x_, double y_) {
		x = x_; y = y_;
	}
	bool operator==(Point const& s) const {
		return x == s.x && y == s.y;
	}
};

struct Circle {
	double x, y, r;
	int i; // just a label
	Circle(double r_, int i_ = 0) {
		r = r_;
		x = 0; y = 0;
		i = i_;
	}
	Circle(double x_, double y_, double r_, int i_ = 0) {
		x = x_; y = y_; r = r_; i = i_;
	}
};

struct Halfplane {
	double A, B, C;
	bool positive;
	Halfplane(double A_, double B_, double C_, bool positive_) {
		A = A_; B = B_; C = C_; positive = positive_;
	}
};

typedef	set<int> Bin;

//compute the distance between two circles;
double distance(Circle const& circle1, Circle const& circle2);

//judge whether the circle whose position is certain is cross the border of bin;
bool cross_the_border(double size_of_bin, bool is_circular_bin, Circle const& circle);
bool cross_the_border_circular(double size_of_bin, Circle const& circle);
bool cross_the_border_square(double size_of_bin, Circle const& circle);

//give you a circles whose position is certain, count the number of tangent point with bin;
int tangent(double size_of_bin, Circle const& circle);

//give you two circles and both of their position are certain, judge whether they are tangent;
bool tangent(Circle const& circle1, Circle const& circle2);

//give you two circles and both of their position are certain, judge whether they are intersect;
bool intersect(Circle const& circle1, Circle const& circle2);
bool intersect(Circle const& circle1, double angle, double size_of_bin);
bool intersectWithLine(Circle const& circle,double angle, double size_of_bin);
bool isInSector(Circle const& circle, double size_of_bin, double angle1, double angle2);

//judge whether the circle is in the halfplane;
bool in_halfplane(Circle const& circle, Halfplane const& halfplane);

// compute the 4 positions of a given circle in the corners of a given bin
vector<Point> get_positions(double size_of_bin, bool is_circular_bin, Circle const& circle);
vector<Point> get_positions_circular(double radius_of_bin, Circle const& circle);
vector<Point> get_positions_square(double size_of_bin, Circle const& circle);

// give you a circle which needs to touch a giving circle and bin, compute the position of the circle and put them to vector;
vector<Point> get_positions(double size_of_bin, bool is_circular_bin, Circle const& circle, Circle const& base_circle);
vector<Point> get_positions_circular(double radius_of_bin, Circle const& circle, Circle const& base_circle);
vector<Point> get_positions_square(double size_of_bin, Circle const& circle, Circle const& base_circle);

//give you a circle which needs to touch two circles, compute the position of the circle and put them to vector;
vector<Point> get_positions_independent(Circle const& circle, Circle const& base_circle1, Circle const& base_circle2);

// returns if circle intersects rectangle defined by [ll,uu]
bool intersect(Circle const& circle, Point const& ll, Point const& uu);

