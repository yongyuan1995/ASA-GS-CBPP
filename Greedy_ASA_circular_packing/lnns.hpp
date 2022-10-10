#pragma once

#include <random>
#include "toa.hpp"

struct LNNS {
    LNNS(double size_of_bin, bool is_circular_bin, vector<double> const& radii, std::vector<int> const& labels) :
        toa(size_of_bin, is_circular_bin, radii, labels) {}
    void operator()();
    vector<Circle> const& get_circles() const { return toa.circles; }
    vector<Bin> const& get_bins() const { return toa.bins; }
    std::vector<std::size_t> sample_bins(std::size_t n);
    std::vector<std::pair<Point, Point>> sample_rects(std::vector<std::size_t> const& bin_ids);
    std::vector<Circle> sample_circles(std::vector<std::size_t> const& bin_ids);

    toa::TOA toa;
    std::random_device rd;
};
