#include "geometry.hpp"

#include <iostream>
#include <cassert>
double const PI = 3.1415926;
const double eps = 1e-12;

//两个圆之间的距离
double distance(Circle const& circle1, Circle const& circle2) {
	double dx = circle1.x - circle2.x;
	double dy = circle1.y - circle2.y;
	return sqrt(dx * dx + dy * dy) - circle1.r - circle2.r;
}

bool cross_the_border(double size_of_bin, bool is_circular_bin, Circle const& circle) {
	return is_circular_bin ?
		cross_the_border_circular(size_of_bin, circle) :
		cross_the_border_square(size_of_bin, circle);
}

bool cross_the_border_square(double size_of_bin, Circle const& circle) {
	if (circle.x - circle.r + eps < 0) return true;
	if (circle.y - circle.r + eps < 0) return true;
	if (size_of_bin - circle.x - circle.r + eps < 0) return true;
	if (size_of_bin - circle.y - circle.r + eps < 0) return true;
	return false;
}

bool cross_the_border_circular(double size_of_bin, Circle const& circle) {
	double c = size_of_bin;
	return std::sqrt(std::pow(circle.x - c, 2.0) + std::pow(circle.y - c, 2.0)) > size_of_bin - circle.r + eps;
}

int tangent(double size_of_bin, Circle const& circle) {
	int ans = 0;
	if (fabs(circle.x - circle.r) < eps) ans++;
	if (fabs(circle.y - circle.r) < eps) ans++;
	if (fabs(circle.x - size_of_bin + circle.r) < eps) ans++;
	if (fabs(circle.y - size_of_bin + circle.r) < eps) ans++;
	return ans;
}
bool tangent(Circle const& circle1, Circle const& circle2) {
	double dx = circle1.x - circle2.x, dy = circle1.y - circle2.y;
	double r = circle1.r + circle2.r;
	if (fabs(dx * dx + dy * dy - r * r) < eps)
		return true;
	return false;
}

bool intersect(Circle const& circle1, Circle const& circle2) {
	//return std::pow(circle1.x-circle2.x,2)+std::pow(circle1.y-circle2.y,2)<std::pow(circle1.r+circle2.r,2);
	double dx = circle1.x - circle2.x, dy = circle1.y - circle2.y;
	double r = circle1.r + circle2.r;
	if (dx * dx + dy * dy + eps < r * r) return true;
	return false;
}
bool intersectWithLine(Circle const& circle, double angle, double size_of_bin)
{
	double theta = angle * PI / 180;
	double R = size_of_bin;
	double C0 = R - circle.x;
	double C1 = R - circle.y;
	double a = 1;
	double b = 2 * C0 * cos(theta) + 2 * C1 * sin(theta);
	double c = C0 * C0 + C1 * C1 - circle.r * circle.r;
	double delta = b * b - 4 * a * c;
	if (delta < 0) return false;
	double t1 = (-1 * b + sqrt(delta)) / 2;
	double t2 = (-1 * b - sqrt(delta)) / 2;
	if (t1 >= 0 || t2 >= 0) return true;
	else return false;
}
bool isInSector(Circle const& circle, double size_of_bin, double angle1, double angle2)
{
	double angle;
	double x = circle.x - size_of_bin;
	double y = circle.y - size_of_bin;
	if (x == 0 && y == 0) return true;
	if (x == 0)
	{
		if (y > 0) angle = 90;
		else angle = 270;
	}
	else
	{
		double theta = atan(y / x);
		if (x > 0)
		{
			if (y > 0) angle = theta * 180 / PI;
			else angle = 360 + theta * 180 / PI;
		}
		else
		{
			if (y > 0) angle = 180 + theta * 180 / PI;
			else angle = 180 + theta * 180 / PI;
		}
	}
	if (angle1 <= angle2)
	{
		if (angle >= angle1 && angle <= angle2) return true;
		else return false;
	}
	else
	{
		if (angle >= angle2 && angle <= 360) return true;
		if (angle >= 0 && angle <= angle1) return true;
		return false;
	}
}
bool intersect(Circle const& circle, double angle, double size_of_bin) {
	double angle1 = angle;
	double angle2 = angle + 40;
	while (angle2 >= 360)
	{
		angle2 -= 360;
	}
	if (intersectWithLine(circle, angle1, size_of_bin)) return true;
	if (intersectWithLine(circle, angle2, size_of_bin)) return true;
	if (isInSector(circle, size_of_bin, angle1, angle2)) return true;
	return false;
}

bool in_halfplane(Circle const& circle, Halfplane const& halfplane) {
	if (halfplane.A * circle.x + halfplane.B * circle.y + halfplane.C > 0) return true;
	return false;
}

vector<Point> get_positions(double size_of_bin, bool is_circular_bin, Circle const& circle) {
	return (is_circular_bin) ?
		get_positions_circular(size_of_bin, circle) :
		get_positions_square(size_of_bin, circle);
}

// compute the 4 positions of a given circle in the corners of a given bin
vector<Point> get_positions_square(double size_of_bin, Circle const& circle) {
	vector<Point> pos;
	pos.push_back(Point(circle.r, circle.r));
	pos.push_back(Point(size_of_bin - circle.r, circle.r));
	pos.push_back(Point(circle.r, size_of_bin - circle.r));
	pos.push_back(Point(size_of_bin - circle.r, size_of_bin - circle.r));
	return pos;
}
// compute the position of a given circle in the edge of a given circular bin
vector<Point> get_positions_circular(double radius_of_bin, Circle const& circle) {
	vector<Point> pos;
	pos.push_back(Point(radius_of_bin, circle.r));
	return pos;
}

vector<Point> get_positions(double size_of_bin, bool is_circular_bin, Circle const& circle, Circle const& base_circle) {
	return (is_circular_bin) ?
		get_positions_circular(size_of_bin, circle, base_circle) :
		get_positions_square(size_of_bin, circle, base_circle);
}

// give you a circle which needs to touch a giving circle and bin, compute the position of the circle and put them to vector;
vector<Point> get_positions_square(double size_of_bin, Circle const& circle, Circle const& base_circle) {
	vector<Point> pos;
	double x, y, dx, dy, r;

	r = circle.r + base_circle.r;

	y = circle.r;
	dy = y - base_circle.y;
	if (r * r >= dy * dy) {
		x = base_circle.x + sqrt(r * r - dy * dy);
		pos.push_back(Point(x, y));
		x = base_circle.x - sqrt(r * r - dy * dy);
		pos.push_back(Point(x, y));
	}
	y = size_of_bin - circle.r;
	dy = y - base_circle.y;
	if (r * r >= dy * dy) {
		x = base_circle.x + sqrt(r * r - dy * dy);
		pos.push_back(Point(x, y));
		x = base_circle.x - sqrt(r * r - dy * dy);
		pos.push_back(Point(x, y));
	}
	x = circle.r;
	dx = x - base_circle.x;
	if (r * r >= dx * dx) {
		y = base_circle.y + sqrt(r * r - dx * dx);
		pos.push_back(Point(x, y));
		y = base_circle.y - sqrt(r * r - dx * dx);
		pos.push_back(Point(x, y));
	}

	x = size_of_bin - circle.r;
	dx = x - base_circle.x;
	if (r * r >= dx * dx) {
		y = base_circle.y + sqrt(r * r - dx * dx);
		pos.push_back(Point(x, y));
		y = base_circle.y - sqrt(r * r - dx * dx);
		pos.push_back(Point(x, y));
	}
	return pos;
}


// give you a circle which needs to touch a giving circle and circular bin, compute the position of the circle and put them to vector;
vector<Point> get_positions_circular(double radius_of_bin, Circle const& circle, Circle const& base_circle) {
	vector<Point> pos;
	double x, y, r, x1, y1, r1, rb;

	x1 = base_circle.x - radius_of_bin; // make center of bin (0,0)
	y1 = base_circle.y - radius_of_bin; // make center of bin (0,0)
	r1 = base_circle.r;
	rb = radius_of_bin;
	r = circle.r;

	double R1 = std::pow(rb - r, 2);
	double R2 = std::pow(r + r1, 2);

	if (x1 == 0 && y1 == 0) {
		y = 0;
		x = radius_of_bin - r;
		pos.push_back(Point(x + radius_of_bin, y + radius_of_bin));
		return pos;
	}
	if (x1 == 0) {
		y = (R1 - R2 + y1 * y1) / (2 * y1);
		if (R1 - y * y >= 0) {
			x = std::sqrt(R1 - y * y);
			pos.push_back(Point(x + radius_of_bin, y + radius_of_bin));
			if (R1 - y * y > 0) {
				x = -std::sqrt(R1 - y * y);
				pos.push_back(Point(x + radius_of_bin, y + radius_of_bin));
			}
		}
		return pos;
	}

	double sq = -R1 * R1 - std::pow(-R2 + x1 * x1 + y1 * y1, 2) + 2 * R1 * (R2 + x1 * x1 + y1 * y1);

	if (sq >= 0) {
		y = (R1 * y1 - R2 * y1 + x1 * x1 * y1 + std::pow(y1, 3) - x1 * std::sqrt(sq)) / (2 * (x1 * x1 + y1 * y1));
		x = -(R2 - x1 * x1 - R1 + 2 * y * y1 - y1 * y1) / (2 * x1);
		pos.push_back(Point(x + radius_of_bin, y + radius_of_bin));
		if (sq > 0) {
			y = (R1 * y1 - R2 * y1 + x1 * x1 * y1 + std::pow(y1, 3) + x1 * std::sqrt(sq)) / (2 * (x1 * x1 + y1 * y1));
			x = -(R2 - x1 * x1 - R1 + 2 * y * y1 - y1 * y1) / (2 * x1);
			pos.push_back(Point(x + radius_of_bin, y + radius_of_bin));
		}
	}
	return pos;
}

//give you a circle which needs to touch two circles, compute the position of the circle and put them to vector;
vector<Point> get_positions_independent(Circle const& circle, Circle const& base_circle1, Circle const& base_circle2) {
	vector<Point> pos;

	double dx1, dx2, dy1, dy2;
	double x1 = base_circle1.x, y1 = base_circle1.y, r1 = base_circle1.r + circle.r;
	double x2 = base_circle2.x, y2 = base_circle2.y, r2 = base_circle2.r + circle.r;

	if (fabs(x1 - x2) < eps) {
		dy1 = (r1 * r1 - r2 * r2 + y2 * y2 - y1 * y1) / 2 / (y2 - y1);
		dy2 = dy1;//(r1*r1-r2*r2+y2*y2-y1*y1)/2/(y2-y1);
		if (r1 * r1 - (dy1 - y1) * (dy1 - y1) < 0) return pos;
		dx1 = x1 + sqrt(r1 * r1 - (dy1 - y1) * (dy1 - y1));
		dx2 = x1 - sqrt(r1 * r1 - (dy1 - y1) * (dy1 - y1));//sqrt(r2*r2-(circle2.y-y2)*(circle2.y-y2))+x2;
	}
	else {
		double a = (r1 * r1 - r2 * r2 + y2 * y2 - y1 * y1 + x2 * x2 - x1 * x1) / 2 / (x2 - x1) - x1;
		double b = (y2 - y1) / (x2 - x1);
		double aa = b * b + 1, bb = -2 * (a * b + y1), cc = a * a + y1 * y1 - r1 * r1;
		double deta = bb * bb - 4 * aa * cc;//(2*a*b+2*y1)*(2*a*b+2*y1)-4*(b*b+1)*(a*a+y1*y1-r1*r1);
		if (deta < 0) return pos;
		dy1 = (-bb + sqrt(deta)) / 2 / aa;//(2*a*b+2*y1+sqrt(deta))/2/(b*b+1);
		dy2 = (-bb - sqrt(deta)) / 2 / aa;//(2*a*b+2*y1-sqrt(deta))/2/(b*b+1);
		dx1 = a - b * dy1 + x1;
		dx2 = a - b * dy2 + x1;
	}
	assert(!isnan(dx1) && !isnan(dx2) && !isnan(dy1) && !isnan(dy2));
	pos.push_back(Point(dx1, dy1));
	pos.push_back(Point(dx2, dy2));
	return pos;
}

// returns if circle intersects rectangle defined by [ll,uu]
bool intersect(Circle const& circle, Point const& ll, Point const& uu) {
	if (circle.x - circle.r > uu.x)
		return false;
	if (circle.y - circle.r > uu.y)
		return false;
	if (circle.x + circle.r < ll.x)
		return false;
	if (circle.y + circle.r < ll.y)
		return false;
	return true;
}
