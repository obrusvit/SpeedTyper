#ifndef GUARD_SPEED_TYPER_TIMER
#define GUARD_SPEED_TYPER_TIMER

#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
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

    std::mutex map_mutex;
    std::map<int, bool> id_2_active;

    auto get_id() { return id_counter++; }

    bool map_check_active(auto id) {
        std::lock_guard<std::mutex> g{map_mutex};
        return id_2_active.at(id);
    }

    void map_remove(auto id) {
        std::lock_guard<std::mutex> g{map_mutex};
        id_2_active.erase(id);
    }

  public:
    [[nodiscard]] static int get_last_id() { return last_id; }

    template <typename Function>
    [[nodiscard]] auto set_timeout(Function func, std::chrono::milliseconds timeout) {
        auto new_id = get_id();
        id_2_active[new_id] = true;
        std::thread t([=, this]() {
            if (!this->map_check_active(new_id)) {
                return;
            }
            std::this_thread::sleep_for(timeout);
            if (this->map_check_active(new_id)) {
                func();
            }
            this->map_remove(new_id);
        });
        t.detach();
        last_id = new_id;
        return new_id;
    }

    /* template <typename Function> */
    /* auto set_interval(Function func, std::chrono::milliseconds interval){ */

    /* } */

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
