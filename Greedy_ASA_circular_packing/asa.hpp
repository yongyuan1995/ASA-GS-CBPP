#pragma once
#include<cmath>
#include <random>
#include "toa.hpp"
struct ASA {
	ASA(double size_of_bin, bool is_circular_bin, vector<double> const& radii, std::vector<int> const& labels) :
		toa(size_of_bin, is_circular_bin, radii, labels) {}
	void operator()();
	vector<Circle> const& get_circles() const { return toa.circles; }
	vector<Bin> const& get_bins() const { return toa.bins; }
	std::vector<std::size_t> sample_bins(std::size_t n);
	std::vector<std::pair<Point, Point>> sample_rects(std::vector<std::size_t> const& bin_ids);
	std::vector<Circle> sample_circles(std::vector<std::size_t> const& bin_ids);
	std::vector<double>sample_sector(std::vector<std::size_t> const& bin_ids);
	bool isAccept(double dE,double t, double N);

	toa::TOA toa;
	std::random_device rd;

};

/*bool judge(double dE, int t)
{
	if (dE < 0)
	{
		return true;
	}
	else
	{
		double d = exp(-1.0 * dE / t);
		if (d > u(e))
		{
			return true;
		}
		else return false;
	}
}
*/