#pragma once

#include "geometry.hpp"
#include <cassert>
#include <unordered_map>

namespace toa {

    struct COA {
        Point p;
        double q0;
        double q1;
        COA(double asize_of_bin, bool is_circular_bin, Point const& ap) : p(ap) {
            if (is_circular_bin) {
                q0 = -std::pow(p.x - asize_of_bin, 2) - std::pow(p.y - asize_of_bin, 2);
                q1 = 0;
            }
            else {
                q0 = min(min(p.x, asize_of_bin - p.x), min(p.y, asize_of_bin - p.y));
                q1 = max(min(p.x, asize_of_bin - p.x), min(p.y, asize_of_bin - p.y));
            }
        }
        // smaller is better
        bool operator< (const COA& v) const {
            if (q0 != v.q0) return q0 < v.q0;
            return q1 < v.q1;
        }
    };

    struct Location {
        Location(std::size_t bin_id, Point const& p) : bin_id(bin_id), p(p) {}
        bool operator==(Location const& l) const {
            return bin_id == l.bin_id && p == l.p;
        }
        std::size_t bin_id;
        Point p;
    };

    struct LocationHasher {
        std::size_t operator()(Location const& l) const { return l.bin_id * 1000 + l.p.x * 11 + l.p.y; }
    };

    struct TOA {
        TOA(double asize_of_bin, bool is_circular_bin, vector<double> const& radii, std::vector<int> const& labels);

        bool check(int bin_id, Circle const& circle) const;
        bool check(int bin_id) const;
        bool check() const;

        std::vector<COA> compute_feasible_coas(int bin_id, int circle_id);
        double get_density(Bin const& bin) const;
        double get_density(std::size_t bin_id) const;
        std::vector<double> get_densities(std::vector<std::size_t> const& bin_ids) const;
        void init();
        void operator()();
        void operator()(std::vector<std::size_t> const& circle_ids);
        bool operator()(std::vector<std::size_t> const& circle_ids, std::vector<std::size_t> const& bin_ids);
        std::set<std::size_t> select_in_rects(std::vector<std::size_t> const& bin_ids, std::vector<std::pair<Point, Point>> const& rects);
        std::set<std::size_t> select_in_circles(std::vector<std::size_t> const& bin_ids, std::vector<Circle> const& cs);
        std::set<std::size_t> select_in_sector(std::vector<std::size_t> const& bin_ids, std::vector<double> const& sector);
        std::set<std::size_t> select_at_most_n(std::vector<std::size_t> const& bin_ids, std::size_t n);
        void relax(std::vector<std::size_t> const& bin_ids, std::set<std::size_t> const& to_remove);
        void sort_bins();
        vector<Circle> const& get_circles() const { return circles; }
        vector<Bin> const& get_bins() const { return bins; }
        bool operator<(TOA const& s) const;
        double promise_density(std::vector<std::size_t> const& bin_ids) const;
        double promise_contact(std::vector<std::size_t> const& bin_ids) const;
        double promise(std::vector<std::size_t> const& bin_ids) const;
        double promise() const;
        double size_of_bin;
        bool is_circular_bin;
        double value_function();
        void bin_clear() { bins.clear(); }
        vector<Circle> circles;
        vector<Bin> bins;
        vector<unordered_map<Location, int, LocationHasher>> taboo;
    };

    std::ostream& operator<<(std::ostream&, TOA const&);
}