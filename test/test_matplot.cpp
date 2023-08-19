#include <catch2/catch_all.hpp>
#include <chrono>
#include <iostream>
#include <matplot/matplot.h>
#include <stdexcept>
#include <vector>

TEST_CASE("TestPlottingWithMatplot") {

    SECTION("PlotWpmInTimeline") {
        const std::vector<float> data = {80.4F, 91.5F, 90.0F};
        using namespace matplot;
        /* auto f1 = matplot::figure(true); */
        auto f = figure(true);
        /* auto ax = f->gca(); */
        auto ax = f->current_axes();
        auto p = ax->plot(data);
        p->color("blue").line_width(3);
        ax->ylabel("CPM");
        ax->xlabel("There will be datetimes..");
        f->draw();
        show();
    }
    SECTION("PlotWpmInDates") {
        using namespace std::chrono;
        using namespace std::chrono_literals;
        const std::vector<float> data = {80.4, 91.5, 90.0};
        const auto t1 = system_clock::now();
        const auto t2 = t1 - 1h;
        const auto t3 = t1 - 2h;
        const std::vector<long> xdata = {0, duration_cast<seconds>(t2-t3).count(), duration_cast<seconds>(t1-t3).count()};

        using namespace matplot;
        /* auto f1 = matplot::figure(true); */
        auto f = figure(true);
        /* auto ax = f->gca(); */
        auto ax = f->current_axes();
        auto p = ax->plot(xdata, data);
        p->color("blue").line_width(3);
        ax->ylabel("CPM");
        ax->xlabel("There will be datetimes..");
        f->draw();
        show();
    }
}
