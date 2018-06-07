#ifndef GUT_LINUX_DEBUGGER_H
#define GUT_LINUX_DEBUGGER_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

namespace gut {

namespace debugger {

bool isAttached() {
    int status = open("/proc/self/status", O_RDONLY);

    if (status == -1)
        return false;

    char buffer[1024];
    ssize_t num_read = read(status, buffer, sizeof(buffer));

    if (num_read > 0) {
        buffer[num_read] = 0;
        static const char prop[] = "TracerPid:";
        char* tracer = strstr(buffer, prop);

        if (tracer)
            return static_cast<bool>(atoi(tracer + sizeof(prop) - 1));
    }
    return false;
}

} // namespace debugger

} // namespace dex

#define GUT_DEBUG_BREAK_ \
    __asm__ volatile("int $0x03");

#endif // GUT_LINUX_DEBUGGER_H
