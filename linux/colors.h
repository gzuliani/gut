#ifndef COLORS_H
#define COLORS_H

namespace gut {

namespace color {

std::ostream& setForegroundColor(std::ostream& os, const char* code) {
    if (ColorInConsole::enabled())
        os << "\x1b[" << code << "m";
    return os;
}

std::ostream& black(std::ostream& os) {
    return setForegroundColor(os, "30");
}

std::ostream& navy(std::ostream& os) {
    return setForegroundColor(os, "34");
}

std::ostream& green(std::ostream& os) {
    return setForegroundColor(os, "32");
}

std::ostream& teal(std::ostream& os) {
    return setForegroundColor(os, "36");
}

std::ostream& maroon(std::ostream& os) {
    return setForegroundColor(os, "31");
}

std::ostream& purple(std::ostream& os) {
    return setForegroundColor(os, "35");
}

std::ostream& olive(std::ostream& os) {
    return setForegroundColor(os, "33");
}

std::ostream& silver(std::ostream& os) {
    return setForegroundColor(os, "37");
}

std::ostream& gray(std::ostream& os) {
    return setForegroundColor(os, "90");
}

std::ostream& blue(std::ostream& os) {
    return setForegroundColor(os, "94");
}

std::ostream& lime(std::ostream& os) {
    return setForegroundColor(os, "92");
}

std::ostream& aqua(std::ostream& os) {
    return setForegroundColor(os, "96");
}

std::ostream& red(std::ostream& os) {
    return setForegroundColor(os, "91");
}

std::ostream& fuchsia(std::ostream& os) {
    return setForegroundColor(os, "95");
}

std::ostream& yellow(std::ostream& os) {
    return setForegroundColor(os, "93");
}

std::ostream& white(std::ostream& os) {
    return setForegroundColor(os, "97");
}

std::ostream& reset(std::ostream& os) {
    return setForegroundColor(os, "0");
}

} // namespace color

} // namespace gut

#endif // COLORS_H
