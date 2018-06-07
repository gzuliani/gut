#ifndef GUT_WINDOWS_TIMINGCUSTOM_H
#define GUT_WINDOWS_TIMINGCUSTOM_H

#include <time.h> // clock

namespace gut {

class Timer {
    clock_t start_;
public:
    Timer() { reset(); }
    void reset() { start_ = clock(); }
    Duration elapsedTime() {
        return Duration(
            static_cast<double>(clock() - start_)
                / CLOCKS_PER_SEC);
    }
};

} // namespace gut

#endif // GUT_WINDOWS_TIMINGCUSTOM_H
