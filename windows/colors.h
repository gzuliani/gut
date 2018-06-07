#ifndef GUT_WINDOWS_COLORS_H
#define GUT_WINDOWS_COLORS_H

#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace gut {

namespace color {

class WindowsConsole_ {
    HANDLE handle_;
    WORD defaultAttrs_;
    static const WORD mask_ =
        FOREGROUND_RED
        | FOREGROUND_GREEN
        | FOREGROUND_BLUE
        | FOREGROUND_INTENSITY;
public:
    WindowsConsole_()
     : handle_(GetStdHandle(STD_OUTPUT_HANDLE))
     , defaultAttrs_(getAttrs()) {
    }
    void setColor(WORD color) {
        if (ColorInConsole::enabled())
            SetConsoleTextAttribute(handle_, (getAttrs() & ~mask_) | color);
    }
    void resetColors() {
        if (ColorInConsole::enabled())
            SetConsoleTextAttribute(handle_, defaultAttrs_);
    }
protected:
    WORD getAttrs() {
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(handle_, &info);
        return info.wAttributes;
    }
};

WindowsConsole_& theConsole() {
    static WindowsConsole_ console;
    return console;
}

void setColor_(WORD color) {
    theConsole().setColor(color);
}

void resetColors_() {
    theConsole().resetColors();
}

std::ostream& black(std::ostream& os) {
    setColor_(0);
    return os;
}

std::ostream& navy(std::ostream& os) {
    setColor_(FOREGROUND_BLUE);
    return os;
}

std::ostream& green(std::ostream& os) {
    setColor_(FOREGROUND_GREEN);
    return os;
}

std::ostream& teal(std::ostream& os) {
    setColor_(FOREGROUND_GREEN | FOREGROUND_BLUE);
    return os;
}

std::ostream& maroon(std::ostream& os) {
    setColor_(FOREGROUND_RED);
    return os;
}

std::ostream& purple(std::ostream& os) {
    setColor_(FOREGROUND_RED | FOREGROUND_BLUE);
    return os;
}

std::ostream& olive(std::ostream& os) {
    setColor_(FOREGROUND_RED | FOREGROUND_GREEN);
    return os;
}

std::ostream& silver(std::ostream& os) {
    setColor_(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return os;
}

std::ostream& gray(std::ostream& os) {
    setColor_(FOREGROUND_INTENSITY);
    return os;
}

std::ostream& blue(std::ostream& os) {
    setColor_(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& lime(std::ostream& os) {
    setColor_(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& aqua(std::ostream& os) {
    setColor_(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& red(std::ostream& os) {
    setColor_(FOREGROUND_RED | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& fuchsia(std::ostream& os) {
    setColor_(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& yellow(std::ostream& os) {
    setColor_(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& white(std::ostream& os) {
    setColor_(
        FOREGROUND_RED
        | FOREGROUND_GREEN
        | FOREGROUND_BLUE
        | FOREGROUND_INTENSITY);
    return os;
}

std::ostream& reset(std::ostream& os) {
    resetColors_();
    return os;
}

} // namespace color

} // namespace gut

#endif // GUT_WINDOWS_COLORS_H
