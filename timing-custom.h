#ifndef GUT_TIMINGCUSTOM_H
#define GUT_TIMINGCUSTOM_H

namespace gut {

class Duration {
    double seconds_;
public:
    explicit Duration(double seconds) : seconds_(seconds) {}
    double seconds() const { return seconds_; }
    bool operator>(double duration) const { return seconds_ > duration; }
};

std::string toString(const Duration& value) {
    std::ostringstream oss;
    oss << value.seconds() << "s";
    return oss.str();
}

template<class T>
Duration match_duration(const Duration& source, const T& /*target*/) {
    return source;
}

template<class T>
Duration match_duration(const T& source, const Duration& /*target*/) {
    return Duration(source);
}

} // namespace gut

#ifdef _WIN32
#include "windows/timing-custom.h"
#elif  __linux__
#include "linux/timing-custom.h"
#endif

#endif // GUT_TIMINGCUSTOM_H
