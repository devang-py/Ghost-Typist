#include "trie.hpp"
#include <algorithm>

Trie::Trie() : root_(std::make_unique<Node>()) {}

void Trie::insert(const std::string& word) {
    Node* current = root_.get();
    for (char c : word) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            auto newNode = std::make_unique<Node>();
            Node* raw = newNode.get();
            current->children[c] = std::move(newNode);
            current = raw;
        } else {
            current = it->second.get();
        }
    }
    current->isEndOfWord = true;
}

void Trie::collect(const Node* node, std::string current, std::vector<std::string>& out, int limit) const {
    if (static_cast<int>(out.size()) >= limit) return;
    if (node->isEndOfWord && !current.empty()) {
        out.push_back(current);
    }
    // Iterate children in a stable (alphabetical) order so suggestions are
    // deterministic rather than depending on hash map iteration order.
    std::vector<char> keys;
    keys.reserve(node->children.size());
    for (const auto& kv : node->children) keys.push_back(kv.first);
    std::sort(keys.begin(), keys.end());

    for (char k : keys) {
        if (static_cast<int>(out.size()) >= limit) return;
        collect(node->children.at(k).get(), current + k, out, limit);
    }
}

std::vector<std::string> Trie::suggest(const std::string& prefix, int limit) const {
    std::vector<std::string> results;
    const Node* current = root_.get();
    for (char c : prefix) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return results; // prefix not in trie at all
        }
        current = it->second.get();
    }
    // current now sits at the node for `prefix` itself; collect completions
    // that lie strictly below it, then trim off the prefix we already have.
    std::vector<std::string> fullWords;
    collect(current, prefix, fullWords, limit + 1); // +1 in case prefix itself is a word
    for (const auto& w : fullWords) {
        if (w != prefix) {
            results.push_back(w.substr(prefix.size()));
        }
        if (static_cast<int>(results.size()) >= limit) break;
    }
    return results;
}
