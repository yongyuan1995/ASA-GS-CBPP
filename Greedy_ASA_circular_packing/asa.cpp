#pragma once
#include<cmath>
#include "asa.hpp"
#include "toa.hpp"


#include <iostream>
std::uniform_real_distribution<double> u(0, 1);
//sample n bins
std::vector<std::size_t> ASA::sample_bins(std::size_t n) {
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
std::vector<std::pair<Point, Point>> ASA::sample_rects(std::vector<std::size_t> const& bin_ids) {
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
std::vector<std::pair<Point, Point>> ASA::sample_rects(std::vector<std::size_t> const& bin_ids) {
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

std::vector<Circle> ASA::sample_circles(std::vector<std::size_t> const& bin_ids) {
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

/*std::vector<double>ASA::sample_sector(std::vector<std::size_t> const& bin_ids)
{
    vector<double>candiate = {0,45,90, 135, 180, 225, 270, 315,360};
    vector<double>sector;
    for (int i = 0; i < bin_ids.size(); i++)
    {
        int k = rand() % candiate.size();
        sector.push_back(candiate[k]);
    }
    return sector;
}*/
std::vector<double>ASA::sample_sector(std::vector<std::size_t> const& bin_ids)
{
    vector<double>sector;
    for (int i = 0; i < bin_ids.size(); i++)
    {
        int k = rand() % 36;
        sector.push_back(k * 10.0);
    }
    return sector;
}
#if 1
void ASA::operator()() {
    std::size_t nb_steps = 600000;
    //std::size_t nb_steps = 6000;
    double alpha = 2500;
    double beta = 0.08;
    double const t_init = 0.1;
   

    double const factor = std::pow(t_init, 1.0 / nb_steps);
    cout << "factor:" << factor << endl;
    //double step = init_t/nb_steps;
    double t = t_init;

    std::size_t const nb_bins_relaxed = 2;
    //toa.init();
    toa();
    toa.sort_bins();
    int N = toa.bins.size();
    double t_cool = (alpha * sqrtf(N) - 1.0) / (alpha * sqrtf(N));
    cout << "t_cool:" << t_cool << endl;

    auto best_solution = toa;
    auto greedy_initial = toa;
    double f_initial = N + 1;
    double f_best = f_initial;
    auto greedy_best = toa;
    int t_greedy = beta * N ;
    int G = 0;
    auto prev_solution = toa;


    for (std::size_t it = 0; it < nb_steps; ++it) {
        assert(toa.check());

        prev_solution = toa;

        assert(toa.check());
        std::vector<std::size_t> bin_ids = sample_bins(std::min(nb_bins_relaxed, toa.bins.size()));
        std::shuffle(bin_ids.begin(), bin_ids.end(), rd);
        /*std::vector<std::size_t> bin_ids = sample_bins(std::min(nb_bins_relaxed, toa.bins.size() - 1));
        
        std::shuffle(bin_ids.begin(), bin_ids.end(), rd);
        bin_ids.push_back(toa.bins.size() - 1);
        */
        

#if 0
        std::vector<std::pair<Point, Point>> rects = sample_rects(bin_ids);
        std::set<std::size_t> removed = gacoa.select_in_rects(bin_ids, rects);
#else   
        bool flag = true;
        if (it % 1000 == 0) {
            flag = !flag;
        }
        std::set<std::size_t> removed;
        if (flag)
        {
            std::vector<double> sectors = sample_sector(bin_ids);
            removed = toa.select_in_sector(bin_ids, sectors);
        }
        else
        {
            std::vector<Circle> circs = sample_circles(bin_ids);
            removed = toa.select_in_circles(bin_ids, circs);
            /*
            std::vector<std::pair<Point, Point>> rects = sample_rects(bin_ids);
            removed = toa.select_in_rects(bin_ids, rects);
            */
        }


#endif
        //std::set<std::size_t> removed = gacoa.select_at_most_n(bin_ids, 3);
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
        // double dE = (gacoa.bins.size() - cur_promise) - (prev_solution.bins.size() + prev_promise);
        //double dE = -1 * (cur_promise - prev_promise);
#else   
        auto prev_densities = prev_solution.get_densities(bin_ids);
        auto cur_densities = gacoa.get_densities(bin_ids);
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
        double dE = (toa.bins.size() - cur_promise) - (prev_solution.bins.size() - prev_promise);
        assert(toa.check());
        //double cur_best_density = std::max(gacoa.get_density(bin1_id), gacoa.get_density(bin2_id));
        /*if (dE < 0 || gacoa.bins.size() < prev_solution.bins.size() ||
            std::bernoulli_distribution(std::exp(-1 * dE / t))(rd)) {;
        }*/
        if (dE <= 0)
        {
            ;
        }
        else
        {
            G = G + 1;
            double f_cur = (toa.bins.size() - cur_promise);
            if (f_cur < f_best)
            {
                f_best = f_cur;
                greedy_best = toa;
            }
            if (G >= t_greedy)
            {
                dE = f_best - (prev_solution.bins.size() - prev_promise);
                if (isAccept(dE, t, N))
                {
                    toa = greedy_best;
                }
            }
            else
            {
                assert(toa.check());
                toa = prev_solution;
                assert(toa.check());
                continue;
            }
        }
        assert(toa.check());
        if (toa < best_solution) {
            for (auto const& bin_id : bin_ids)
                std::cout << bin_id << " ";
            best_solution = toa;
            std::cout << "improved: " << best_solution << std::endl;
        }
        if (it % 5000 == 0) {

            cout << "step:" << it << endl;
        }
        t *= t_cool;
        G = 0;
        f_best = f_initial;
        //t -= step;
    }
    toa = best_solution;
}
#else

void ASA::operator()() {
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

bool ASA::isAccept(double dE, double t, double N)
{
    if (dE < 0)
    {
        return true;
    }
    else
    {
       double d = exp(-1.0 * dE / t * log(N));
        if (d > u(rd))
        {
            return true;
        }
        /*
        if (std::bernoulli_distribution(std::exp(-1 * dE / t))(rd))
        {
            return true;
        }
        */
        else return false;
    }
}