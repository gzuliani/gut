#ifndef COLORS_H
#define COLORS_H

#include <iostream>
#include <windows.h>

namespace color
{

class Console_ {
	HANDLE handle_;
	WORD defaultAttrs_;
	static const WORD mask_ = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
public:
	Console_()
	 : handle_(GetStdHandle(STD_OUTPUT_HANDLE))
	 , defaultAttrs_(getAttrs()) {
	}
	void setColor(WORD color) {
		SetConsoleTextAttribute(handle_, (getAttrs() & ~mask_) | color);
	}
	void resetColors() {
		SetConsoleTextAttribute(handle_, defaultAttrs_);
	}
protected:
	WORD getAttrs() {
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(handle_, &info);
		return info.wAttributes;
	}
};

Console_& theConsole() {
	static Console_ console;
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
	setColor_(FOREGROUND_GREEN|FOREGROUND_BLUE);
	return os;
}

std::ostream& maroon(std::ostream& os) {
	setColor_(FOREGROUND_RED);
	return os;
}

std::ostream& purple(std::ostream& os) {
	setColor_(FOREGROUND_RED|FOREGROUND_BLUE);
	return os;
}

std::ostream& olive(std::ostream& os) {
	setColor_(FOREGROUND_RED|FOREGROUND_GREEN);
	return os;
}

std::ostream& silver(std::ostream& os) {
	setColor_(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
	return os;
}

std::ostream& gray(std::ostream& os) {
	setColor_(FOREGROUND_INTENSITY);
	return os;
}

std::ostream& blue(std::ostream& os) {
	setColor_(FOREGROUND_BLUE|FOREGROUND_INTENSITY);
	return os;
}

std::ostream& lime(std::ostream& os) {
	setColor_(FOREGROUND_GREEN|FOREGROUND_INTENSITY);
	return os;
}

std::ostream& aqua(std::ostream& os) {
	setColor_(FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
	return os;
}

std::ostream& red(std::ostream& os) {
 setColor_(FOREGROUND_RED|FOREGROUND_INTENSITY);
	return os;
}

std::ostream& fuchsia(std::ostream& os) {
	setColor_(FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
	return os;
}

std::ostream& yellow(std::ostream& os) {
	setColor_(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
	return os;
}

std::ostream& white(std::ostream& os) {
	setColor_(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
	return os;
}

std::ostream& reset(std::ostream& os) {
	resetColors_();
	return os;
}

} // namespace color

#endif // COLORS_H
