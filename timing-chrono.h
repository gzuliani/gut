#ifndef GUT_TIMINGCHRONO_H
#define GUT_TIMINGCHRONO_H

#include <chrono>

namespace gut {

class Duration {
    typedef std::chrono::duration<double> duration_t;
    duration_t seconds_;
public:
    explicit Duration(double seconds) : seconds_(seconds) {}
    template<class Rep, class Period>
    explicit Duration(std::chrono::duration<Rep, Period> duration)
     : seconds_(std::chrono::duration_cast<duration_t>(duration)) {}
    double seconds() const { return seconds_.count(); }
    template<class T>
    bool operator>(const T& duration) const { return seconds_ > duration; }
    bool operator>(int duration) const { return seconds_.count() > duration; }
    bool operator>(double duration) const {
        return seconds_.count() > duration;
    }
};

class Timer {
    std::chrono::steady_clock::time_point start_;
public:
    Timer() { reset (); }
    void reset() { start_ = std::chrono::steady_clock::now(); }
    Duration elapsedTime() {
        return Duration(std::chrono::steady_clock::now() - start_);
    }
};

std::string toString(const Duration& value) {
    std::ostringstream oss;
    oss << value.seconds() << "s";
    return oss.str();
}

template<class T, class U>
Duration match_duration(const T& source, const U& /*target*/) {
    return Duration(source);
}

} // namespace gut

#endif // GUT_TIMINGCHRONO_H
