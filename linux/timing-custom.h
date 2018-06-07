#ifndef GUT_LINUX_TIMINGCUSTOM_H
#define GUT_LINUX_TIMINGCUSTOM_H

#include <sys/times.h> // times
#include <unistd.h>    // _SC_CLK_TCK

namespace gut {

class Timer {
    clock_t start_;
public:
    Timer() { reset(); }
    void reset() {
        struct tms start;
        start_ = times(&start);
    }
    Duration elapsedTime() {
        struct tms end;
        return Duration(
            static_cast<double>(times(&end) - start_)
                / sysconf(_SC_CLK_TCK));
    }
};

} // namespace gut

#endif // GUT_LINUX_TIMINGCUSTOM_H
