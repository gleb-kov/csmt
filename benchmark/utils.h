#ifndef CSMT_UTILS_H
#define CSMT_UTILS_H

#include <chrono>
#include <string>
#include <random>

namespace bench_utils {
    void escape(void *p) {
        asm volatile("" : : "g"(p) : "memory");
    }

    void clobber() {
        asm volatile("" : : : "memory");
    }

    template<typename T>
    inline void do_not_optimize(const T &val) {
        asm volatile("" : : "r,m"(val) : "memory");
    }

    template<typename T>
    inline void do_not_optimize(T &val) {
        asm volatile("" : "+m,r"(val) : : "memory");
    }
}

namespace time_utils {

    using benchmark_clock_t = std::chrono::high_resolution_clock;
    using benchmark_duration_t = std::chrono::milliseconds;

    template<typename Clock = benchmark_clock_t>
    class stage_timer {
        using point_t = std::chrono::time_point<Clock>;

        const point_t init_point_;
        point_t snapshot_;

    public:
        /* util functions that might be useful and compact */

        [[nodiscard]] static point_t now() noexcept {
            return Clock::now();
        }

        template<typename T>
        [[nodiscard]] static auto duration(point_t finish, point_t start) {
            return std::chrono::duration_cast<T>(finish - start);
        }

        template<typename T>
        [[nodiscard]] static auto duration_since(point_t point) {
            return duration<T>(now(), point);
        }

    public:
        stage_timer() noexcept
                : init_point_(now()) {
            start_stage();
        }

        point_t get_stage_start() const noexcept {
            return snapshot_;
        }

        point_t start_stage() noexcept {
            snapshot_ = now();
            return snapshot_;
        }

        template<typename T = benchmark_duration_t>
        [[nodiscard]] auto since_init() const {
            return duration_since<T>(init_point_);
        }

        /* stage is still going, do not update snapshot */
        template<typename T = benchmark_duration_t>
        [[nodiscard]] auto get_duration() const {
            return duration_since<T>(snapshot_);
        }

        /* stop stage and return duration */
        template<typename T = benchmark_duration_t>
        auto stop_stage() {
            point_t helper = now();
            std::swap(helper, snapshot_);
            return duration<T>(snapshot_, helper);
        }
    };

}

namespace string_utils {
    std::string generate_random_string(size_t size) {
        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distrib(0, 26 - 1 + 26 - 1 + 10 - 1);

        std::string result;
        for (size_t i = 0; i < size; ++i) {
            int mchar = distrib(generator);
            if (mchar < 10) {
                result += (char) ('0' + mchar);
            } else if (mchar < 36) {
                result += (char) ('a' - 10 + mchar);
            } else {
                result += (char) ('A' - 36 + mchar);
            }
        }
        return result;
    }
}

#endif // CSMT_UTILS_H
