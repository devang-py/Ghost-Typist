#ifndef GHOSTTYPIST_TRIE_HPP
#define GHOSTTYPIST_TRIE_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

// ---------------------------------------------------------------------------
// Trie: prefix tree used for live autocomplete suggestions.
//
// Each node holds a map of next-character -> child node. Insert walks/builds
// a path character by character; suggest(prefix) walks down to the node for
// that prefix, then does a small DFS to find completions.
// ---------------------------------------------------------------------------
class Trie {
public:
    Trie();

    void insert(const std::string& word);

    // Returns up to `limit` completions (the part AFTER the prefix) sorted
    // shortest-first, so the most immediate completion comes first.
    std::vector<std::string> suggest(const std::string& prefix, int limit = 1) const;

private:
    struct Node {
        std::unordered_map<char, std::unique_ptr<Node>> children;
        bool isEndOfWord = false;
    };

    void collect(const Node* node, std::string current, std::vector<std::string>& out, int limit) const;

    std::unique_ptr<Node> root_;
};

#endif // GHOSTTYPIST_TRIE_HPP
