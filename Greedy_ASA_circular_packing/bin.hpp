
#include <vector>
#include <cmath>
#include <functional>
#include <queue>
#include <algorithm>

using namespace std;
extern const double eps;

struct Circle;
struct Halfplane;

struct COA {
	int tangent_point;
	double distance_to_candidates;
	COA() {}
	COA(int tangent_point_, double distance_to_candidates_) {
		tangent_point = tangent_point_;
		distance_to_candidates = distance_to_candidates_;
	}
	bool operator < (const COA& v) const {
		if (tangent_point != v.tangent_point) return tangent_point < v.tangent_point;
		return distance_to_candidates > v.distance_to_candidates;
	}
	// the higher COA the better
	bool operator > (const COA& v) const {
		if (tangent_point != v.tangent_point) return tangent_point > v.tangent_point;
		return distance_to_candidates < v.distance_to_candidates;
	}
};

struct POA {
	double x, y;
	POA() {}
	POA(double x_, double y_) {
		x = min(x_, y_); y = max(x_, y_);
	}
	bool operator < (const POA& v) const {
		if (fabs(x - v.x) > eps) return x > v.x;
		return y > v.y;
	}
	bool operator > (const POA& v) const {
		if (fabs(x - v.x) > eps) return x < v.x;
		return y < v.y;
	}
	bool operator != (const POA& v) const {
		if (fabs(x - v.x) > eps) return true;
		if (fabs(y - v.y) > eps) return true;
		return false;
	}
	bool operator == (const POA& v) const {
		if (fabs(x - v.x) > eps) return false;
		if (fabs(y - v.y) > eps) return false;
		return true;
	}
};

struct Action {
	int bin_id;
	int circle_id;
	double x, y;
	COA coa;
	POA poa;
	//Action(){}
	Action(int bin_id_, int circle_id_, double x_, double y_) : bin_id(bin_id_), circle_id(circle_id_) {
		x = x_; y = y_;
	}
	Action(int bin_id_, int circle_id_, double x_, double y_, COA coa_, POA poa_) : bin_id(bin_id_), circle_id(circle_id_) {
		x = x_; y = y_;
		coa = coa_; poa = poa_;
	}
	// if action A compares smaller than action B, then A will be preferred
	bool operator < (const Action& v) const {
		if (poa != v.poa) return poa < v.poa;
		return coa < v.coa;
	}
	bool operator > (const Action& v) const {
		if (poa != v.poa) return poa > v.poa;
		return coa > v.coa;
	}
};

//give you a circle whose position is certain and a bin, judge whether it is legal for the circle;
bool check(int bin_id, Circle const& circle);

//keep the priority_queue have most top_k elements;
void maintain_priority_queue(priority_queue<Action, vector<Action>, greater<Action> >& action_q);

//compute value of reasonable actions and put them to priority_queue;
void filter_and_sort_actions(vector<Action> const& pos, priority_queue<Action, vector<Action>, greater<Action> >& action_q);

//give you a circle which needs to touch two borders of bin, compute the position of the circle and put them to vector;
void get_position(int bin_id, int circle_id, vector<Action >& pos);

// give you a circle which needs to touch a giving circle and bin, compute the position of the circle and put them to vector;
void get_position(int bin_id, int circle_id, int base_circle_id, vector<Action >& pos);

//give you a circle which needs to touch two giving circles, compute the position of the circle,and put them to vector;
void get_position(int bin_id, int circle_id, int base_circle1_id, int base_circle2_id, vector<Action >& pos);

//get all halfplanes about a action;
void get_halfplane(int bin_id, Action const& action, vector<Halfplane>& halfplane);

// give you a action, conut COA of the action;
COA get_COA(int bin_id, Action const& action);

// give you a action, count POA of the action;
POA get_POA(int bin_id, Action const& action);

//give you a circle and a bin, get all actions about putting circle to bin and put them to priority_queue;
void get_action(int bin_id, int circle_id, priority_queue<Action, vector<Action>, greater<Action> >& action_q);

//give you some circles and empty bins, put all circles to these bins;
//if we can put all circles to these bins, return true, otherwise return false;
bool work();
#pragma once
