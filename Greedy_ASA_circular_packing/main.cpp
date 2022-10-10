#pragma once
#include <stdio.h>
#include "bin.hpp"
#include "geometry.hpp"
#include "toa.hpp"
#include "lnns.hpp"
#include "asa.hpp"
#include<time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
using namespace std;

const double eps = 1e-12;
const int inf = 1e9;
int top_k;
double size_of_bin;
bool is_circular_bin = true;
double const M_PI = 3.14159265;
double max_area = 0;
int number_of_circles;

vector<Circle > circles;
vector<Bin > bins;

void show(vector<Circle>& circle) {
	puts("circles:");
	for (int i = 0; i < (int)circle.size(); i++)
		printf("%.4lf %.4lf %.4lf\n", circle[i].x, circle[i].y, circle[i].r);
	puts("");
	return;
}

//void main_nifei();
void main_toa();
void main_toa_lnns();
void main_RL();
void main_toa_asa();
void swap(std::vector<std::size_t>& circle_ids, int i, int j, int k)
{
	size_t tmp = circle_ids[k * i];
	for (int b = 0; b < k - 1; b++)
	{
		circle_ids[k * i + b] = circle_ids[k * j + b];
		circle_ids[k * j + b] = circle_ids[k * i + b + 1];
	}

	circle_ids[k * j + k - 1] = tmp;
}
void swap(std::vector<std::size_t>& circle_ids, int i, int j)
{
	size_t tmp = circle_ids[i];
	circle_ids[i] = circle_ids[j];
	circle_ids[j] = tmp;
}

double get_density(Bin const& bin) {
	double a = 0;
	for (auto const& circle_id : bin)
		a += M_PI * pow(circles[circle_id].r, 2);
	return a / pow(size_of_bin, 2.0);
}

int main(int argc, char** argv) {

	if (argc != 2) {
		std::cerr << "usage: " << argv[0] << " output_file.solution\n";
		return 1;
	}
	string file = "toa_sqrt_cbpp_circular_random_8_40.solution";
	clock_t start, finish;
	double totaltime;
	//int number_of_circles;
	cin >> size_of_bin >> number_of_circles;

	std::vector<double> array_radius(number_of_circles);
	for (int i = 0; i < number_of_circles; i++)
		cin >> array_radius[i];

	sort(array_radius.begin(), array_radius.end());

	int j = 1;
	circles.push_back(Circle(array_radius[0], j));
	for (int i = 1; i < number_of_circles; i++) {
		if (array_radius[i] != array_radius[i - 1]) {
			j = j + 1;
		}
		circles.push_back(Circle(array_radius[i], j));
	}

	start = clock();
	//main_kevin();
	//main_toa();
	//main_toa_lnns();
	////main_RL();
	main_toa_asa();
	finish = clock();

	// sort bins by decreasing density
	sort(bins.begin(), bins.end(), [&](Bin const& bin1, Bin const& bin2) {
		return get_density(bin1) > get_density(bin2);
		});

	printf("number of circles:%d\n", number_of_circles);
	printf("the size of bin:%lf\n", size_of_bin);
	printf("the total numbers of bin %d\n", (int)bins.size());
	for (int i = 0; i < (int)bins.size(); i++) {
		printf("the number of circles in bin%d: %d\n", i + 1, (int)bins[i].size());
		printf("center of x, center of y,  radius,    label \n");
		double area_occupied = 0;
		double ratio = 0;
		for (auto const& circle_id : bins[i]) {
			printf(" %10.4lf %10.4lf %10.4lf %10d\n", circles[circle_id].x, circles[circle_id].y, circles[circle_id].r, circles[circle_id].i);
			area_occupied += M_PI * circles[circle_id].r * circles[circle_id].r;
			ratio += 1.0 / circles[circle_id].r;
		}
		if (area_occupied > max_area) {
			max_area = area_occupied;
		}
		if (is_circular_bin)
		{
			printf("density: %.4lf\n", area_occupied / (M_PI * size_of_bin * size_of_bin));
		}
		else
		{
			printf("density: %.4lf\n", area_occupied / (M_PI * size_of_bin * size_of_bin));
		}

		printf("ratio: %.4lf\n", ratio);

	}

	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("max area:%.4lf \n", max_area);
	printf("total time:%.4lf s\n", totaltime);
	printf("finish.\n");

	std::ofstream sol(argv[1]);
	//std::ofstream sol(file);
	sol << "{ \"numberOfCircles\": " << number_of_circles << ", "
		<< "  \"numberOfBins\": " << bins.size() << ", "
		<< "  \"binsSize\": " << size_of_bin << ", "
		<< "  \"circles\": [[" << circles[0].x << "," << circles[0].y << "," << circles[0].r << "," << circles[0].i << "]";
	for (std::size_t i = 1; i < circles.size(); ++i)
		sol << ",[" << circles[i].x << "," << circles[i].y << "," << circles[i].r << "," << circles[i].i <<"]";
	std::vector<int> bin_of_circle(circles.size());
	for (std::size_t bin_id = 0; bin_id < bins.size(); ++bin_id)
		for (int circle_id : bins[bin_id])
			bin_of_circle[circle_id] = bin_id;
	sol << "],\"bins\": [" << bin_of_circle[0];
	for (std::size_t i = 1; i < circles.size(); ++i)
		sol << "," << bin_of_circle[i];
	sol << "],\"totalTime\": " << totaltime << "}";
	sol.close();
	return 0;
}

#if 0
// Check if the position of the circle is legal, whether it conflicts with the box or other circles
bool check(int bin_id, Circle const& circle) {
	if (cross_the_border(size_of_bin, circle)) return false;
	for (auto const& circle_id : bins[bin_id]) {
		if (intersect(circle, circles[circle_id])) return false;
	}
	return true;
}

//keep the priority_queue have most top_k elements;
void maintain_priority_queue(priority_queue<Action >& action_q) {
	while ((int)action_q.size() > top_k) action_q.pop();
}

// compute the 4 positions of a given circle in the corners of a given bin
void get_position(int bin_id, int circle_id, vector<Action >& pos) {
	for (auto const& p : get_positions(size_of_bin, circles[circle_id]))
		pos.push_back(Action(bin_id, circle_id, p.x, p.y));
}

// give you a circle which needs to touch a giving circle and bin, compute the position of the circle and put them to vector;
void get_position(int bin_id, int circle_id, int base_circle_id, vector<Action >& pos) {
	for (auto const& p : get_positions(size_of_bin, circles[circle_id], circles[base_circle_id]))
		pos.push_back(Action(bin_id, circle_id, p.x, p.y));
}

//give you a circle which needs to touch two circles, compute the position of the circle and put them to vector;
void get_position(int bin_id, int circle_id, int base_circle1_id, int base_circle2_id, vector<Action >& pos) {
	for (auto const& p : get_positions(circles[circle_id], circles[base_circle1_id], circles[base_circle2_id]))
		pos.push_back(Action(bin_id, circle_id, p.x, p.y));
}

void get_halfplane(int bin_id, Action const& action, vector<Halfplane>& halfplane) {
	Circle circle = Circle(action.x, action.y, circles[action.circle_id].r);
	for (auto const& circle_id : bins[bin_id]) {
		if (!tangent(circle, circles[circle_id])) continue;
		double A, B, C;
		double x1 = circle.x, y1 = circle.y, r1 = circle.r;
		double x2 = circles[circle_id].x, y2 = circles[circle_id].y, r2 = circles[circle_id].r;
		if (fabs(x1 * y2 - x2 * y1) < eps) {
			if (fabs(x1 - x2) < eps) A = 1, B = 0;
			else if (fabs(y1 - y2) < eps) A = 0, B = 1;
			else {
				A = 1;
				if (x1 == 0) B = A * x2 / y2;
				else B = A * x1 / y1;
			}
		}
		else {
			A = (y1 - y2) / (x2 * y1 - x1 * y2);
			B = (x1 - x2) / (x2 * y1 - x1 * y2);
		}
		double xmid = x1 + r1 / (r1 + r2) * (x2 - x1);
		double ymid = y1 + r1 / (r1 + r2) * (y2 - y1);
		C = B * xmid - A * ymid;
		halfplane.push_back(Halfplane(-B, A, C, A * x1 + B * y1 + C > 0));
		halfplane.push_back(Halfplane(-B, A, C, A * x1 + B * y1 + C < 0));
	}
}

COA get_COA(int bin_id, Action const& action) {
	Circle circle = Circle(action.x, action.y, circles[action.circle_id].r);
	COA coa;
	coa.tangent_point = 200 * tangent(size_of_bin, circle);
	for (auto const& circle_id : bins[bin_id]) {
		if (tangent(circle, circles[circle_id])) coa.tangent_point++;
	}
	coa.distance_to_candidates = inf;

	vector<Halfplane > halfplane;
	get_halfplane(bin_id, action, halfplane);

	for (auto it1 = bins[bin_id].begin(); it1 != bins[bin_id].end(); ++it1) {
		bool flag = true;
		for (auto it2 = std::next(it1); it2 != bins[bin_id].end(); ++it2)
			if (!in_halfplane(circles[*it1], halfplane[std::distance(bins[bin_id].begin(), it2)])) flag = false;
		if (!flag) continue;
		coa.distance_to_candidates = min(coa.distance_to_candidates, distance(circle, circles[*it1]));
	}

	halfplane.clear();
	return coa;
}

POA get_POA(int, Action const& action) {
	POA poa = POA(action.x, action.y);
	poa = max(poa, POA(action.x, size_of_bin - action.y));
	poa = max(poa, POA(size_of_bin - action.x, action.y));
	poa = max(poa, POA(size_of_bin - action.x, size_of_bin - action.y));
	return poa;
}

void filter_and_sort_actions(vector<Action> const& pos, priority_queue<Action >& action_q) {
	for (int i = 0; i < (int)pos.size(); i++) {
		Circle circle = Circle(pos[i].x, pos[i].y, circles[pos[i].circle_id].r);
		if (check(pos[i].bin_id, circle)) {
			Action action(pos[i]);
			action.coa = get_COA(pos[i].bin_id, pos[i]);
			action.poa = get_POA(pos[i].bin_id, pos[i]);
			action_q.push(action);
		}
	}

	maintain_priority_queue(action_q); //       ???????????????????????????
}

void get_action(int bin_id, int circle_id, priority_queue<Action >& action_q) {
	vector<Action > pos;
	get_position(bin_id, circle_id, pos);
	for (auto it1 = bins[bin_id].begin(); it1 != bins[bin_id].end(); ++it1)
		get_position(bin_id, circle_id, *it1, pos);
	for (auto it1 = bins[bin_id].begin(); it1 != bins[bin_id].end(); ++it1) {
		for (auto it2 = std::next(it1); it2 != bins[bin_id].end(); ++it2)
			get_position(bin_id, circle_id, *it1, *it2, pos);
	}
	filter_and_sort_actions(pos, action_q);
}

bool work() {
	priority_queue<Action > action_q;
	std::vector<bool> choose(circles.size(), false);

	for (imnt pp = 0; pp < (int)circles.size(); pp++) {

		// collect possible actions for the largest circle that is not yet placed
		for (int bin_id = 0; bin_id < (int)bins.size(); bin_id++) {
			for (int circle_id = (int)circles.size() - 1; circle_id >= 0; circle_id--) {
				if (!choose[circle_id]) get_action(bin_id, circle_id, action_q);
				if (!action_q.empty())	break;
			}
		}
		if (action_q.empty()) return false;
		Action best_action = action_q.top();
		while (!action_q.empty()) {
			if (action_q.top().bin_id < best_action.bin_id)
			{
				best_action = action_q.top();
			}
			else if (action_q.top().bin_id == best_action.bin_id)
			{
				if (action_q.top().coa > best_action.coa) {
					best_action = action_q.top();
				}
			}

			//			printf("%.2lf %.2lf %.2lf %d\n",action_q.top().x,action_q.top().y,action_q.top().circle->r,action_q.top().bin->bin_id);
			action_q.pop();
		}
		circles[best_action.circle_id].x = best_action.x;
		circles[best_action.circle_id].y = best_action.y;
		choose[best_action.circle_id] = true;
		bins[best_action.bin_id].insert(best_action.circle_id);
		//		printf("best_action: %.2lf %.2lf %.2lf %d\n",best_action.circle->x,best_action.circle->y,best_action.circle->r,best_action.bin->bin_id);
		//		show(circle);

	}
	//show(circle);
	for (int i = 0; i < (int)circles.size(); i++)
		if (!choose[i]) return false;
	return true;
}

void main_kevin() {
	// searches for the least possible number of bins 
	for (std::size_t i = 1; i <= circles.size(); i++) {
		for (std::size_t j = 0; j < i; j++)
			bins.push_back({});
		//top_k = i*8*2;
		top_k = (int)circles.size();
		//printf("bin_number = %d\n",i);
		//printf("%d\n",(int)bin.size());
//		for(int j=0;j<i;j++)
//			printf("bin_id=%d\n",bin[j].bin_id);
		bool found_solution = work();
		//printf("%d\n",(int)flag );
		if (found_solution) {
			//puts("tail");
			break;
		}
		//show(circle);
		//puts("tail");
		// if we can use i bins to achieve our goal,then exit.

		bins.clear();
	}
}
#endif

void main_toa() {
	std::vector<double> radii;
	std::vector<int> labels;
	for (auto const& circle : circles) {
		radii.push_back(circle.r);
		labels.push_back(circle.i);
	}
	toa::TOA toa(size_of_bin, is_circular_bin, radii, labels);
	toa();
	circles = toa.get_circles();
	bins = toa.get_bins();
	cout << toa.value_function() << endl;
}

void main_toa_lnns() {
	std::vector<double> radii;
	std::vector<int> labels;

	for (auto const& circle : circles) {
		radii.push_back(circle.r);
		labels.push_back(circle.i);
	}
	LNNS lnns(size_of_bin, is_circular_bin, radii, labels);
	lnns();
	circles = lnns.get_circles();
	bins = lnns.get_bins();
}
void main_toa_asa() {
	std::vector<double> radii;
	std::vector<int> labels;

	for (auto const& circle : circles) {
		radii.push_back(circle.r);
		labels.push_back(circle.i);
	}
	ASA asa(size_of_bin, is_circular_bin, radii, labels);
	asa();
	circles = asa.get_circles();
	bins = asa.get_bins();
}

void main_RL() {
	//std::random_device rdd;
	//std::mt19937 gen(rdd());
	std::vector<double> radii;
	std::vector<int> labels;
	for (auto const& circle : circles) {
		radii.push_back(circle.r);
		labels.push_back(circle.i);
	}

	toa::TOA toa(size_of_bin, is_circular_bin, radii, labels);
	toa::TOA best_toa = toa;
	std::vector<std::size_t> circle_ids;
	for (std::size_t circle_id = 0; circle_id < circles.size(); ++circle_id)
		circle_ids.push_back(circle_id);

	toa(circle_ids);
	cout << toa.value_function() << endl;
	best_toa = toa;
	srand((unsigned)time(NULL));
	for (int epoch = 0; epoch < 1000; epoch++)
	{
		toa.bin_clear();
		/*if ((epoch + 1) % 100 == 0)
		{
			//srand((unsigned)time(NULL));
			//std::random_shuffle(circle_ids.begin(), circle_ids.end() -20);
		}*/
		int ii = rand() % (number_of_circles / 2);
		int jj = rand() % (number_of_circles / 2);

		while (abs(ii - jj) < 2)
		{
			jj = rand() % (number_of_circles / 2);
		}
		//cout << "ii:"  << ii<< "jj:" <<  jj << endl;
		swap(circle_ids, ii, jj);
		toa(circle_ids);
		//cout << toa.value_function() << endl;
		double dE = -1 * (toa.value_function() - best_toa.value_function());
		//if (judge(dE * 50, 1))
		if(1)
		{
			if (dE < 0) {
				best_toa = toa;
				cout << toa.value_function() << endl;
			}

		}
		else
		{
			swap(circle_ids, ii, jj);
		}

	}
	circles = best_toa.get_circles();
	bins = best_toa.get_bins();

}