#include "lnns.hpp"
#include "toa.hpp"

#include <iostream>

//sample n bins
std::vector<std::size_t> LNNS::sample_bins(std::size_t n) {
    std::vector<std::size_t> r;
    for (std::size_t i = 0; i < n; ++i) {
        std::size_t lb = 0;
        if (!r.empty())
            lb = r.back() + 1;
        std::uniform_int_distribution<std::size_t> d(lb, toa.bins.size() - n + i);
        r.push_back(d(rd));
    }
    return r;
}

#if 0
//sample a rect
std::vector<std::pair<Point, Point>> LNNS::sample_rects(std::vector<std::size_t> const& bin_ids) {
    std::uniform_real_distribution<double> dd(0, toa.size_of_bin);
    double dx, dy;
    dx = dd(rd);
    dy = dd(rd);
    std::vector<std::pair<Point, Point>> r;
    for (std::size_t i = 0; i < bin_ids.size(); ++i) {
        std::uniform_real_distribution<double> lxd(0, toa.size_of_bin - dx);
        std::uniform_real_distribution<double> lyd(0, toa.size_of_bin - dy);
        double lx = lxd(rd);
        double ly = lyd(rd);
        r.push_back(std::make_pair(Point(lx, ly), Point(lx + dx, ly + dy)));
    }
    return r;
}
#else
//This function takes xyz  does a,b,c and returns def
std::vector<std::pair<Point, Point>> LNNS::sample_rects(std::vector<std::size_t> const& bin_ids) {
    std::uniform_real_distribution<double>  dd(0, toa.size_of_bin);
    double dx, dy;
    dx = dd(rd);
    dy = dd(rd);
    std::vector<std::pair<Point, Point>> r;
    for (auto const& bin_id : bin_ids) {
        double lx, ly;
        lx = ly = 0;
        if (!toa.bins[bin_id].empty()) {
            auto circle_id = *std::next(toa.bins[bin_id].begin(),
                std::uniform_int_distribution<std::size_t>(0, toa.bins[bin_id].size() - 1)(rd));
            lx = toa.circles[circle_id].x - dx / 2;
            ly = toa.circles[circle_id].y - dy / 2;
        }
        r.push_back(std::make_pair(Point(lx, ly), Point(lx + dx, ly + dy)));
    }
    return r;
}
#endif

std::vector<Circle> LNNS::sample_circles(std::vector<std::size_t> const& bin_ids) {
    std::uniform_real_distribution<double> dr(0, toa.size_of_bin / 2);
    double r = dr(rd);

    std::vector<Circle> ret;
    for (auto const& bin_id : bin_ids) {
        auto circle_id = *std::next(toa.bins[bin_id].begin(),
            std::uniform_int_distribution<std::size_t>(0, toa.bins[bin_id].size() - 1)(rd));
        Circle c(toa.circles[circle_id].x, toa.circles[circle_id].y, r);
        ret.push_back(c);
    }
    return ret;
}

#if 1
void LNNS::operator()() {
    //std::size_t nb_steps = 200000;
    std::size_t nb_steps = 300000;
    double const init_t = 0.1;
    double const factor = std::pow(init_t, 1.0 / nb_steps);
    //double step = init_t/nb_steps;
    double t = init_t;

    std::size_t const nb_bins_relaxed = 2;
    toa.init();
    toa.sort_bins();

    auto best_solution = toa;

    for (std::size_t it = 0; it < nb_steps; ++it) {
        assert(toa.check());

        auto const prev_solution = toa;

        assert(toa.check());//?????????????
        std::vector<std::size_t> bin_ids = sample_bins(std::min(nb_bins_relaxed, toa.bins.size()));
        std::shuffle(bin_ids.begin(), bin_ids.end(), rd);
#if 0
        std::vector<std::pair<Point, Point>> rects = sample_rects(bin_ids);
        std::set<std::size_t> removed = toa.select_in_rects(bin_ids, rects);
#else
        std::vector<Circle> circs = sample_circles(bin_ids);
        std::set<std::size_t> removed = toa.select_in_circles(bin_ids, circs);
#endif
        //std::set<std::size_t> removed = toa.select_at_most_n(bin_ids, 3);
        //Relax repair and repark for the best move
        toa.relax(bin_ids, removed);
        std::vector<std::size_t> removed_v(removed.begin(), removed.end());
        //std::shuffle(removed_v.begin(), removed_v.end(), rd);
        if (!toa(removed_v, bin_ids)) {
            toa = prev_solution;
            continue;
        }

#if 1
        double prev_promise = prev_solution.promise(bin_ids);
        double cur_promise = toa.promise(bin_ids);
        double improvement = cur_promise - prev_promise;
#else
        auto prev_densities = prev_solution.get_densities(bin_ids);
        auto cur_densities = toa.get_densities(bin_ids);
        double improvement = *std::min_element(prev_densities.begin(), prev_densities.end()) -
            *std::min_element(cur_densities.begin(), cur_densities.end());
        if (improvement == 0)
            improvement = *std::max_element(cur_densities.begin(), cur_densities.end()) -
            *std::max_element(prev_densities.begin(), prev_densities.end());
#endif        
        toa.sort_bins();
        std::size_t non_empty_count = 0;
        for (std::size_t bin_id = 0; bin_id < toa.bins.size(); ++bin_id)
            if (toa.get_density(bin_id) > 0)
                ++non_empty_count;
        toa.bins.resize(non_empty_count);
        assert(toa.check());
        //double cur_best_density = std::max(toa.get_density(bin1_id), toa.get_density(bin2_id));
        if (improvement > 0 || toa.bins.size() < prev_solution.bins.size() ||
            std::bernoulli_distribution(std::exp(improvement / t))(rd)) {
            //if (improvement<0)
            //    std::cout << improvement << " " << t << " " << std::exp(improvement/t) << std::endl;
        }
        else {
            assert(toa.check());
            toa = prev_solution;
            assert(toa.check());
        }
        assert(toa.check());
        if (toa < best_solution) {
            for (auto const& bin_id : bin_ids)
                std::cout << bin_id << " ";
            best_solution = toa;
            std::cout << "improved: " << best_solution << std::endl;
        }
        t *= factor;
        //t -= step;
    }
    toa = best_solution;
}
#else

void LNNS::operator()() {
    std::size_t nb_steps = 100000;

    std::size_t const nb_bins_relaxed = 3;
    std::size_t const max_taboo_embargo = 100;
    toa.init();
    toa.sort_bins();

    auto best_solution = toa;

    for (std::size_t it = 0; it < nb_steps; ++it) {
        assert(toa.check());

        auto const prev_solution = toa;

        assert(toa.check());
        std::vector<std::size_t> bin_ids = sample_bins(std::min(nb_bins_relaxed, toa.bins.size()));
        std::shuffle(bin_ids.begin(), bin_ids.end(), rd);
#if 1
        std::vector<std::pair<Point, Point>> rects = sample_rects(bin_ids);
        std::set<std::size_t> removed = toa.select_in_rects(bin_ids, rects);
#else
        std::vector<Circle> circs = sample_circles(bin_ids);
        std::set<std::size_t> removed = toa.select_in_circles(bin_ids, circs);
#endif
        //std::set<std::size_t> removed = toa.select_at_most_n(bin_ids, 3);
        toa.relax(bin_ids, removed);
        std::vector<std::size_t> removed_v(removed.begin(), removed.end());
        //std::shuffle(removed_v.begin(), removed_v.end(), rd);
        if (!toa(removed_v, bin_ids)) {
            toa = prev_solution;
            continue;
        }
        for (auto& s : toa.taboo)
            for (auto it = s.begin(); it != s.end(); ) {
                if (--it->second == 0)
                    s.erase(it++);
                else
                    ++it;
            }
        for (auto bin_id : bin_ids)
            for (auto circle_id : toa.bins[bin_id]) {
                Dosh::Location l(bin_id, Point(toa.circles[circle_id].x, toa.circles[circle_id].y));
                auto it = toa.taboo[circle_id].find(l);
                if (it != toa.taboo[circle_id].end())
                    it->second = max_taboo_embargo;
                else
                    toa.taboo[circle_id].insert(std::make_pair(l, max_taboo_embargo));
            }

        if (toa < best_solution) {
            for (auto const& bin_id : bin_ids)
                std::cout << bin_id << " ";
            best_solution = toa;
            std::cout << "improved: " << best_solution << std::endl;
        }
    }
    toa = best_solution;
}
#endif