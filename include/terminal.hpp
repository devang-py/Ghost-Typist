#ifndef GHOSTTYPIST_TERMINAL_HPP
#define GHOSTTYPIST_TERMINAL_HPP

#include <string>

// ---------------------------------------------------------------------------
// Terminal: thin wrapper around raw-mode input + ANSI escape codes.
//
// Responsibilities:
//   - Put the terminal into "raw mode" so keystrokes are delivered the
//     instant they are pressed (no waiting for Enter, no local echo).
//   - Restore the terminal to its normal state on exit (RAII via destructor,
//     so even an exception or early return can't leave the user's shell
//     broken).
//   - Provide small helpers for colored / dim / cleared output so the rest
//     of the program never has to hardcode escape sequences.
// ---------------------------------------------------------------------------
class Terminal {
public:
    Terminal();
    ~Terminal();

    // Non-blocking-ish single character read. Returns -1 if no key is
    // currently available (used so we can keep redrawing the ghost even
    // when the user hasn't pressed anything yet).
    int readKeyNonBlocking();

    // Blocking single character read (used for simple menu prompts).
    int readKeyBlocking();

    static void moveCursorToLineStart();
    static void clearToEndOfLine();
    static void hideCursor();
    static void showCursor();

    // Color helpers — wrap `text` in the matching ANSI codes and reset
    // afterwards so styling never leaks into the next piece of text.
    static std::string green(const std::string& text);
    static std::string red(const std::string& text);
    static std::string dim(const std::string& text);
    static std::string dimHighlight(const std::string& text);
    static std::string yellow(const std::string& text);
    static std::string grey(const std::string& text);

private:
    void enableRawMode();
    void disableRawMode();

    bool rawModeEnabled_;
#ifndef _WIN32
    void* savedTermios_; // opaque pointer to struct termios, hides <termios.h> from the header
#endif
};

#endif // GHOSTTYPIST_TERMINAL_HPP
