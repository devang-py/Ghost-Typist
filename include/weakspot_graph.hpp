#ifndef GHOSTTYPIST_WEAKSPOT_GRAPH_HPP
#define GHOSTTYPIST_WEAKSPOT_GRAPH_HPP

#include <string>
#include <unordered_map>

// ---------------------------------------------------------------------------
// WeakSpotGraph: tracks which characters the user tends to mistype AS the
// character that should have come next.
//
// Modeled as a directed, weighted graph: nodes are characters, and an edge
// expected->typed with weight N means "N times, when the user should have
// typed `expected`, they actually typed `typed` instead." This naturally
// supports multiple exercises feeding the same graph over time, which is
// what lets it spot a recurring weak spot rather than a one-off typo.
// ---------------------------------------------------------------------------
class WeakSpotGraph {
public:
    void recordMistake(char expected, char typedInstead);

    // Returns the single most frequent mistake pattern as a readable string
    // (e.g. "you often type 'e' instead of 'i'"), or an empty string if no
    // mistakes have been recorded yet.
    std::string worstPattern() const;

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;

private:
    // adjacency_[expected][typedInstead] = count
    std::unordered_map<char, std::unordered_map<char, int>> adjacency_;
};

#endif // GHOSTTYPIST_WEAKSPOT_GRAPH_HPP
