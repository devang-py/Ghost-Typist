#ifndef GHOSTTYPIST_BRACKET_STACK_HPP
#define GHOSTTYPIST_BRACKET_STACK_HPP

#include <string>

// ---------------------------------------------------------------------------
// BracketStack: validates that (), [], {} are balanced in whatever the user
// has typed SO FAR, re-checked on every keystroke.
//
// This is a textbook use of a stack: walk the typed text left to right,
// push every opening bracket, and on a closing bracket, pop and confirm it
// matches. Any mismatch is reported immediately rather than waiting for the
// user to finish the whole line.
// ---------------------------------------------------------------------------
class BracketStack {
public:
    // Returns a human-readable warning ("unmatched ')'", "2 bracket(s)
    // still open", ...) or an empty string if everything typed so far is
    // valid (even if incomplete).
    static std::string checkLive(const std::string& typedSoFar);
};

#endif // GHOSTTYPIST_BRACKET_STACK_HPP
