#include "gut.h"

GUT_ENABLE_BREAKINDEBUGGER

TEST("break into debugger") {
    int i = 1;
    CHECK(i == 2);
}
