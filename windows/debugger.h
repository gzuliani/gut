#ifndef GUT_WINDOWS_DEBUGGER_H
#define GUT_WINDOWS_DEBUGGER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace gut {

namespace debugger {

bool isAttached() {
    return IsDebuggerPresent() == TRUE;
}

} // namespace debugger

} // namespace dex

#define GUT_DEBUG_BREAK_ \
    __debugbreak()

#endif // GUT_WINDOWS_DEBUGGER_H
