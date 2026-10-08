#include "terminal.hpp"
#include <cstdio>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>
    #include <sys/time.h>
#endif

Terminal::Terminal() : rawModeEnabled_(false)
#ifndef _WIN32
    , savedTermios_(nullptr)
#endif
{
    enableRawMode();
}

Terminal::~Terminal() {
    disableRawMode();
    showCursor();
    std::cout.flush();
}

void Terminal::enableRawMode() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(hOut, &mode)) {
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    rawModeEnabled_ = true;
#else
    struct termios original;
    if (tcgetattr(STDIN_FILENO, &original) != 0) return;

    savedTermios_ = new struct termios(original);

    struct termios raw = original;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    rawModeEnabled_ = true;
#endif
}

void Terminal::disableRawMode() {
    if (!rawModeEnabled_) return;
#ifndef _WIN32
    if (savedTermios_) {
        tcsetattr(STDIN_FILENO, TCSANOW, static_cast<struct termios*>(savedTermios_));
        delete static_cast<struct termios*>(savedTermios_);
        savedTermios_ = nullptr;
    }
#endif
    rawModeEnabled_ = false;
}

int Terminal::readKeyNonBlocking() {
#ifdef _WIN32
    if (_kbhit()) {
        int c = _getch();
        if (c == 0 || c == 224) {
            _getch();
            return -1;
        }
        return c;
    }
    return -1;
#else
    unsigned char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n <= 0) return -1;
    return static_cast<int>(c);
#endif
}

int Terminal::readKeyBlocking() {
    int key = -1;
    while (key == -1) {
        key = readKeyNonBlocking();
    }
    return key;
}

void Terminal::moveCursorToLineStart() {
    std::cout << "\r";
}

void Terminal::clearToEndOfLine() {
    std::cout << "\x1b[K";
}

void Terminal::hideCursor() {
    std::cout << "\x1b[?25l";
}

void Terminal::showCursor() {
    std::cout << "\x1b[?25h";
}

std::string Terminal::green(const std::string& text) {
    return "\x1b[32m" + text + "\x1b[0m";
}

std::string Terminal::red(const std::string& text) {
    return "\x1b[31m" + text + "\x1b[0m";
}

std::string Terminal::dim(const std::string& text) {
    return "\x1b[2m" + text + "\x1b[0m";
}

std::string Terminal::dimHighlight(const std::string& text) {
    return "\x1b[2m\x1b[44m" + text + "\x1b[0m";
}

std::string Terminal::yellow(const std::string& text) {
    return "\x1b[33m" + text + "\x1b[0m";
}

std::string Terminal::grey(const std::string& text) {
    return "\x1b[90m" + text + "\x1b[0m";
}
