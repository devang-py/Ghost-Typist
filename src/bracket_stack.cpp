#include "bracket_stack.hpp"
#include <stack>
#include <unordered_map>

std::string BracketStack::checkLive(const std::string& typedSoFar) {
    static const std::unordered_map<char, char> closesFor = {
        {')', '('}, {']', '['}, {'}', '{'}
    };

    std::stack<char> openBrackets;

    for (char c : typedSoFar) {
        if (c == '(' || c == '[' || c == '{') {
            openBrackets.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (openBrackets.empty() || openBrackets.top() != closesFor.at(c)) {
                return std::string("unmatched '") + c + "'";
            }
            openBrackets.pop();
        }
    }

    if (!openBrackets.empty()) {
        return std::to_string(openBrackets.size()) + " bracket(s) still open";
    }
    return ""; // balanced so far
}
