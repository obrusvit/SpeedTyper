#ifndef GUARD_SPEED_TYPER_TIMER
#define GUARD_SPEED_TYPER_TIMER

#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace speedtyper {

/**
 * Simple Timer Class.
 * Inspired by https://github.com/99x/timercpp but my Timer returns IDs of new tasks
 * and you can disable them.
 *
 * Drawback: Timer uses detached threads that must access the class member id_2_active.
 *           The access is guarded but Timer object must outlive the life of all threads.
 *           This should be OK because There will 'global' Timer for SpeedTyper game.
 */
class Timer {

  private:
    std::atomic_int id_counter{0};
    static int last_id;

    std::mutex map_mutex; // FIXME make one mutex for each map
    std::map<int, bool> id_2_active;
    std::map<int, int> id_2_remaining_s;

    auto gen_new_id() { return id_counter++; }

    bool map_check_active(auto id) {
        std::lock_guard<std::mutex> g{map_mutex};
        try {
            return id_2_active.at(id);
        } catch (const std::out_of_range& e) {
            return false;
        }
    }

    void maps_remove(auto id) {
        std::lock_guard<std::mutex> g{map_mutex};
        id_2_active.erase(id);
        id_2_remaining_s.erase(id);
    }

    void map_remaining_decrement(auto id) {
        std::lock_guard<std::mutex> g{map_mutex};
        try {
            --id_2_remaining_s.at(id);
        } catch (const std::out_of_range& e) {
            // nothing is needed to be done
        }
    }

  public:
    [[nodiscard]] static int get_last_id() { return last_id; }

    auto get_remaining_time_s(auto id, auto default_val) {
        std::lock_guard<std::mutex> g{map_mutex};
        try {
            if (id_2_active.at(id)) {
                return id_2_remaining_s.at(id);
            }
            return default_val;
        } catch (const std::out_of_range& e) {
            return default_val;
        }
    }

    auto set_interval(int timer_id, std::chrono::milliseconds interval) {
        std::thread t([=, this]() {
            while (true) {
                if (!this->map_check_active(timer_id)) {
                    return;
                }
                std::this_thread::sleep_for(interval);
                if (this->map_check_active(timer_id)) {
                    map_remaining_decrement(timer_id);
                }
            }
        });
        t.detach();
    }

    template <typename Function>
    [[nodiscard]] auto set_timeout(Function func, std::chrono::milliseconds timeout) {
        auto new_id = gen_new_id();
        id_2_active[new_id] = true;
        id_2_remaining_s[new_id] =
            static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(timeout).count());
        std::thread t([=, this]() {
            if (!this->map_check_active(new_id)) {
                return;
            }
            std::this_thread::sleep_for(timeout);
            if (this->map_check_active(new_id)) {
                func();
            }
            this->maps_remove(new_id);
        });
        t.detach();
        set_interval(new_id, std::chrono::milliseconds{1000});
        last_id = new_id;
        return new_id;
    }

    auto disable(auto id) {
        std::lock_guard<std::mutex> g{map_mutex};
        if (id_2_active.contains(id)) {
            id_2_active[id] = false;
        }
    }
};

int Timer::last_id = 0;

} // namespace speedtyper

#endif /* ifndef GUARD_SPEED_TYPER_TIMER */
