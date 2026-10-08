#include "ghost.hpp"
#include <fstream>
#include <sstream>

namespace {

std::string escapeText(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '\n') out += "\\n";
        else if (c == '\\') out += "\\\\";
        else out += c;
    }
    return out;
}

std::string unescapeText(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            ++i;
            out += (s[i] == 'n') ? '\n' : s[i];
        } else if (s[i] != '\r') {
            out += s[i];
        }
    }
    return out;
}

}

GhostRun::GhostRun(std::string exerciseText, std::vector<KeystrokeEvent> events, int wpm)
    : exerciseText_(std::move(exerciseText)), events_(std::move(events)), wpm_(wpm) {}

std::queue<KeystrokeEvent> GhostRun::replayQueue() const {
    std::queue<KeystrokeEvent> q;
    for (const auto& e : events_) {
        q.push(e);
    }
    return q;
}

bool GhostRun::saveToFile(const std::string& path) const {
    std::ofstream out(path.c_str(), std::ios::trunc);
    if (!out.is_open()) return false;

    out << wpm_ << "\n";
    out << escapeText(exerciseText_) << "\n";
    for (const auto& e : events_) {
        out << e.typedLength << "," << e.elapsedMs << "\n";
    }
    out.flush();
    return out.good();
}

bool GhostRun::loadFromFile(const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in.is_open()) return false;

    std::string wpmLine;
    if (!std::getline(in, wpmLine)) return false;
    try {
        wpm_ = std::stoi(wpmLine);
    } catch (...) {
        return false;
    }

    std::string textLine;
    if (!std::getline(in, textLine)) return false;
    exerciseText_ = unescapeText(textLine);

    events_.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string lenStr, msStr;
        if (std::getline(iss, lenStr, ',') && std::getline(iss, msStr)) {
            try {
                events_.push_back(KeystrokeEvent{std::stoi(lenStr), std::stoll(msStr)});
            } catch (...) {
                continue;
            }
        }
    }
    return true;
}
