#ifndef GUT_DEBUGGER_H
#define GUT_DEBUGGER_H

#include "utils.h"

namespace gut {

namespace debugger {

// enable breakpoints
struct BreakInDebugger_ {};
typedef StaticFlag<BreakInDebugger_> BreakInDebugger;

#define GUT_ENABLE_BREAKINDEBUGGER \
    gut::debugger::BreakInDebugger breakInDebugger_;

} // namespace debugger

} // namespace gut

#ifdef _WIN32

    #include "windows/debugger.h"

#elif __linux__

    #include "linux/debugger.h"

#endif

#ifdef NDEBUG

    #define GUT_DEBUG_BREAK

#else

    #define GUT_DEBUG_BREAK \
        if (gut::debugger::BreakInDebugger::enabled() \
            && gut::debugger::isAttached()) \
            GUT_DEBUG_BREAK_;

#endif

#endif // GUT_DEBUGGER_H
