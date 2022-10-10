#include "toa.hpp"
#include <iostream>
#include <set>
#include <numeric>
#include <random>
#include<iterator>


double const eps = 1e-14;
double const M_PI = 3.1415926;

namespace toa {

    TOA::TOA(double asize_of_bin, bool ais_circular_bin, vector<double> const& radii, std::vector<int> const& labels) :
        size_of_bin(asize_of_bin), is_circular_bin(ais_circular_bin), taboo(radii.size()) {

        std::vector<int> p(radii.size());
        std::iota(p.begin(), p.end(), 0);
        std::sort(p.begin(), p.end(), [&](auto i, auto j) { return radii[i] > radii[j]; });

        for (auto const& i : p)
            circles.push_back(Circle(radii[i], labels[i]));
    }

    // Check if the position of the circle is legal, whether it conflicts with the box or other circles
    bool TOA::check(int bin_id, Circle const& circle) const {
        if (cross_the_border(size_of_bin, is_circular_bin, circle))
            return false;
        for (auto const& circle_id : bins[bin_id])
            if (intersect(circle, circles[circle_id]))
                return false;
        return true;
    }

    // Check if all positions of all circles in this bin are legal
    bool TOA::check(int bin_id) const {
        for (auto it1 = bins[bin_id].begin(); it1 != bins[bin_id].end(); ++it1) {
            if (cross_the_border(size_of_bin, is_circular_bin, circles[*it1]))
                return false;
            for (auto it2 = std::next(it1); it2 != bins[bin_id].end(); ++it2)
                if (intersect(circles[*it1], circles[*it2]))
                    return false;
        }
        return true;
    }

    // Check if all positions of all circles in all bins are legal
    bool TOA::check() const {
        for (std::size_t bin_id = 0; bin_id < bins.size(); ++bin_id)
            if (!check(bin_id))
                return false;
        return true;
    }

    std::vector<COA> TOA::compute_feasible_coas(int bin_id, int circle_id) {
        std::vector<Point> pos = get_positions(size_of_bin, is_circular_bin, circles[circle_id]);
        for (auto const& base_circle_id : bins[bin_id]) {
            auto p = get_positions(size_of_bin, is_circular_bin, circles[circle_id], circles[base_circle_id]);
            pos.insert(pos.end(), p.begin(), p.end());
        }
        for (auto it1 = bins[bin_id].begin(); it1 != bins[bin_id].end(); ++it1)
            for (auto it2 = std::next(it1); it2 != bins[bin_id].end(); ++it2) {
                auto p = get_positions_independent(circles[circle_id], circles[*it1], circles[*it2]);
                pos.insert(pos.end(), p.begin(), p.end());
            }
        std::vector<COA> coas;
        for (std::size_t i = 0; i < pos.size(); ++i)
            if (check(bin_id, Circle(pos[i].x, pos[i].y, circles[circle_id].r))) {
                /*auto tbr = taboo[circle_id].find(Location(bin_id, Point(pos[i].x, pos[i].y)));
                if (tbr != taboo[circle_id].end())
                    continue;*/
                coas.push_back(COA(size_of_bin, is_circular_bin, pos[i]));

            }
        return coas;
    }

    double TOA::get_density(Bin const& bin) const {
        double a = 0;
        for (auto const& circle_id : bin)
            a += M_PI * pow(circles[circle_id].r, 2);
        if (is_circular_bin)
            return a / (M_PI * size_of_bin * size_of_bin);
        else
            return a / pow(size_of_bin, 2.0);
    }

    double TOA::get_density(std::size_t bin_id) const {
        double a = 0;
        for (auto const& circle_id : bins[bin_id])
            a += M_PI * pow(circles[circle_id].r, 2);
        if (is_circular_bin)
            return a / (M_PI * size_of_bin * size_of_bin);
        else
            return a / pow(size_of_bin, 2.0);
    }

    std::vector<double> TOA::get_densities(std::vector<std::size_t> const& bin_ids) const {
        std::vector<double> r;
        for (auto const& bin_id : bin_ids)
            if (bin_id >= bins.size())
                r.push_back(0);
            else
                r.push_back(get_density(bin_id));
        return r;
    }

    void TOA::sort_bins() {
        std::sort(bins.begin(), bins.end(), [&](Bin const& bin1, Bin const& bin2) {
            return get_density(bin1) > get_density(bin2);
            });
    }

    void TOA::operator()() {
        std::set<std::size_t> circle_ids;
        for (std::size_t circle_id = 0; circle_id < circles.size(); ++circle_id)
            circle_ids.insert(circle_id);
        for (auto const& bin_circle_ids : bins)
            for (std::size_t bin_circle_id : bin_circle_ids)
                circle_ids.erase(bin_circle_id);
        std::vector<std::size_t> v_circle_ids(circle_ids.begin(), circle_ids.end());
        operator()(v_circle_ids);
    }

    void TOA::init() {
        if (is_circular_bin)
            for (std::size_t circle_id = 0; circle_id < circles.size(); ++circle_id) {
                bins.push_back({ (int)circle_id });
                circles[circle_id].x = size_of_bin;
                circles[circle_id].y = circles[circle_id].r;
            }
        else
            for (std::size_t circle_id = 0; circle_id < circles.size(); ++circle_id) {
                bins.push_back({ (int)circle_id });
                circles[circle_id].x = circles[circle_id].r;
                circles[circle_id].y = circles[circle_id].r;
            }
    }

    // circle_ids is the ids of the circles yet to place
    void TOA::operator()(std::vector<std::size_t> const& circle_ids) {
#if 1
        assert(check());
        for (std::size_t circle_id : circle_ids) {
            std::vector<COA> s;
            std::size_t bin_id = 0;
            while (true) {
                if (bin_id == bins.size())
                    bins.push_back({});
                s = compute_feasible_coas(bin_id, circle_id);
                if (!s.empty())
                    break;
                ++bin_id;
            }
            assert(!s.empty());
            COA best_coa = *std::min_element(s.begin(), s.end());
            circles[circle_id].x = best_coa.p.x;
            circles[circle_id].y = best_coa.p.y;
            bins[bin_id].insert(circle_id);
            assert(check(bin_id));
        }
        assert(check());
#endif  
    }

    // circle_ids is the ids of the circles yet to place
    // bin_ids is the bins to use (if can't place returns false)
    // bin_ids also determines the order in which to try to place circles
    bool TOA::operator()(std::vector<std::size_t> const& circle_ids, std::vector<std::size_t> const& bin_ids) {
        assert(check());
        for (std::size_t circle_id : circle_ids) {
            std::vector<COA> s;
            std::size_t bin_id_idx = 0;
            while (true) {
                if (bin_id_idx == bin_ids.size())
                    return false;
                s = compute_feasible_coas(bin_ids[bin_id_idx], circle_id);
                if (!s.empty())
                    break;
                ++bin_id_idx;
            }
            assert(!s.empty());
            COA best_toa = *std::min_element(s.begin(), s.end());
            /*int selected = rand() % ((int)s.size());
            COA best_coa = s[selected];*/
            circles[circle_id].x = best_toa.p.x;
            circles[circle_id].y = best_toa.p.y;
            bins[bin_ids[bin_id_idx]].insert(circle_id);
        }
        assert(check());
        return true;
    }

    std::set<std::size_t> TOA::select_in_rects(std::vector<std::size_t> const& bin_ids, std::vector<std::pair<Point, Point>> const& rects) {
        std::set<std::size_t> selected;
        for (std::size_t bin_id_idx = 0; bin_id_idx < bin_ids.size(); ++bin_id_idx) {
            std::size_t bin_id = bin_ids[bin_id_idx];
            for (std::size_t circle_id : bins[bin_id]) {
                if (intersect(circles[circle_id], rects[bin_id_idx].first, rects[bin_id_idx].second))
                    selected.insert(circle_id);
            }
        }
        return selected;
    }

    std::set<std::size_t> TOA::select_in_circles(std::vector<std::size_t> const& bin_ids, std::vector<Circle> const& cs) {
        std::set<std::size_t> selected;
        for (std::size_t bin_id_idx = 0; bin_id_idx < bin_ids.size(); ++bin_id_idx) {
            std::size_t bin_id = bin_ids[bin_id_idx];
            for (std::size_t circle_id : bins[bin_id]) {
                if (intersect(circles[circle_id], cs[bin_id_idx]))
                    selected.insert(circle_id);
            }
        }
        return selected;
    }
    std::set<std::size_t> TOA::select_in_sector(std::vector<std::size_t> const& bin_ids, std::vector<double> const& sector)
    {
        std::set<std::size_t> selected;
        for (std::size_t bin_id_idx = 0; bin_id_idx < bin_ids.size(); ++bin_id_idx) {
            std::size_t bin_id = bin_ids[bin_id_idx];
            for (std::size_t circle_id : bins[bin_id]) {
                if (intersect(circles[circle_id], sector[bin_id_idx],size_of_bin))
                    selected.insert(circle_id);
            }
        }
        return selected;

    }
    std::set<std::size_t> TOA::select_at_most_n(std::vector<std::size_t> const& bin_ids, std::size_t n) {
        static std::random_device rd;
        std::set<std::size_t> selected;
        for (std::size_t bin_id_idx = 0; bin_id_idx < bin_ids.size(); ++bin_id_idx) {
            std::size_t bin_id = bin_ids[bin_id_idx];
            std::vector<std::size_t> bin_copy(bins[bin_id].begin(), bins[bin_id].end());
            std::shuffle(bin_copy.begin(), bin_copy.end(), rd);
            for (std::size_t i = 0; i < std::min(n, bins[bin_id].size()); ++i)
                selected.insert(bin_copy[i]);
        }
        return selected;
    }

    void TOA::relax(std::vector<std::size_t> const& bin_ids, std::set<std::size_t> const& to_remove) {
        assert(check());
        std::set<std::size_t> removed;
        for (std::size_t bin_id_idx = 0; bin_id_idx < bin_ids.size(); ++bin_id_idx) {
            std::size_t bin_id = bin_ids[bin_id_idx];
            std::set<int> new_bin;
            std::set_difference(bins[bin_id].begin(), bins[bin_id].end(),
                to_remove.begin(), to_remove.end(), std::inserter(new_bin, new_bin.end()));
            bins[bin_id] = new_bin;
        }
        assert(check());
    }

    bool TOA::operator<(TOA const& s) const {
        if (bins.size() != s.bins.size())
            return bins.size() < s.bins.size();
        for (int bin_id = bins.size() - 1; bin_id >= 0; --bin_id) {
            double d1 = get_density(bin_id);
            double d2 = s.get_density(bin_id);
            if (std::abs(d1 - d2) > eps)
                return d1 < d2;
        }
        return false;
    }

    // promise: higher is better
    double TOA::promise_density(std::vector<std::size_t> const& bin_ids) const {
        auto densities = get_densities(bin_ids);
        double max_d = *max_element(densities.begin(), densities.end());
        double min_d = *min_element(densities.begin(), densities.end());
        return max_d - min_d;
    }

    // maximize contact points: not as good as above
    double TOA::promise_contact(std::vector<std::size_t> const& bin_ids) const {
        std::size_t c = 0;
        std::size_t cc = 0;
        for (auto bin_id : bin_ids) {
            for (auto it1 = bins[bin_id].begin(); it1 != bins[bin_id].end(); ++it1) {
                for (auto it2 = std::next(it1); it2 != bins[bin_id].end(); ++it2)
                    c += intersect(circles[*it1], circles[*it2]);
            }
            cc += bins[bin_id].size();
        }
        return c / (double)cc;
    }

    double TOA::promise(std::vector<std::size_t> const& bin_ids) const {
        //return promise_density(bin_ids)*10+promise_contact(bin_ids);
        return promise_density(bin_ids);
    }

    double TOA::promise() const {
        std::vector<std::size_t> bin_ids(bins.size());
        std::iota(bin_ids.begin(), bin_ids.end(), 0);
        return promise(bin_ids);
    }
    double TOA::value_function()
    {

        double max_density = 0;
        double min_density = 1;
        double density;
        for (int i = 0; i < (int)bins.size(); i++) {
            double area_occupied = 0;
            for (auto const& circle_id : bins[i]) {
                area_occupied += M_PI * circles[circle_id].r * circles[circle_id].r;
            }
            density = area_occupied / (size_of_bin * size_of_bin);
            if (max_density < density) max_density = density;
            if (min_density > density) min_density = density;
        }
        double value = (-1) * ((double)bins.size()) + max_density - min_density;
        return value;
    }

    std::ostream& operator<<(std::ostream& os, TOA const& s) {
        os << s.bins.size();
        for (std::size_t i = 0; i < s.bins.size(); ++i)
            os << "\t" << s.get_density(i);
        return os;
    }

}
