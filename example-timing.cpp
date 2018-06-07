#include "gut/gut.h"

#include <thread>

int f(int, int) {
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    return 0;
}

TEST("duration detection") {
    // define a threshold in s
    auto limit_s = std::chrono::duration<double>(.1);

    // emits a warning
    SHOULD_LAST_AT_MOST(f(1, 2), limit_s);

    // emits an error
    LASTS_AT_MOST(f(1, 2), limit_s);

    // can use threshold defined in other units
    auto limit_ms = std::chrono::milliseconds(100);

    LASTS_AT_MOST(f(1, 2), limit_ms);

    // thresholds are in s by default
    LASTS_AT_MOST(f(1, 2),  0);
    LASTS_AT_MOST(f(1, 2),  1);
    LASTS_AT_MOST(f(1, 2), .1);

    // causes the test to end
    REQUIRE_LASTS_AT_MOST(f(1, 2), limit_s);

    // this check won't be executed
    CHECK(1 == 2);
}
