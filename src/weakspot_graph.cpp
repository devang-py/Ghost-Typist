#include "weakspot_graph.hpp"
#include <fstream>
#include <sstream>

void WeakSpotGraph::recordMistake(char expected, char typedInstead) {
    if (expected == typedInstead) return; // not actually a mistake
    adjacency_[expected][typedInstead] += 1;
}

std::string WeakSpotGraph::worstPattern() const {
    char bestExpected = 0, bestTyped = 0;
    int bestCount = 0;

    for (const auto& fromEntry : adjacency_) {
        for (const auto& toEntry : fromEntry.second) {
            if (toEntry.second > bestCount) {
                bestCount = toEntry.second;
                bestExpected = fromEntry.first;
                bestTyped = toEntry.first;
            }
        }
    }

    if (bestCount == 0) return "";

    std::ostringstream oss;
    oss << "you often type '" << bestTyped << "' instead of '" << bestExpected
        << "' (" << bestCount << " time" << (bestCount == 1 ? "" : "s") << ")";
    return oss.str();
}

bool WeakSpotGraph::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        if (line.size() < 3) continue;
        // format: "<expected>,<typed>,<count>"
        std::istringstream iss(line);
        std::string a, b, c;
        if (std::getline(iss, a, ',') && std::getline(iss, b, ',') && std::getline(iss, c)) {
            if (a.empty() || b.empty()) continue;
            try {
                adjacency_[a[0]][b[0]] = std::stoi(c);
            } catch (...) {
                continue;
            }
        }
    }
    return true;
}

bool WeakSpotGraph::saveToFile(const std::string& path) const {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) return false;

    for (const auto& fromEntry : adjacency_) {
        for (const auto& toEntry : fromEntry.second) {
            out << fromEntry.first << "," << toEntry.first << "," << toEntry.second << "\n";
        }
    }
    return true;
}
