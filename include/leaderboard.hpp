#ifndef GHOSTTYPIST_LEADERBOARD_HPP
#define GHOSTTYPIST_LEADERBOARD_HPP

#include <string>
#include <vector>
#include <memory>

// ---------------------------------------------------------------------------
// Leaderboard: a Binary Search Tree keyed on WPM, storing which exercise
// achieved that speed. An in-order traversal yields every personal best
// sorted fastest-to-slowest without needing to re-sort anything.
// ---------------------------------------------------------------------------
class Leaderboard {
public:
    struct Entry {
        int wpm;
        std::string exercise;
    };

    Leaderboard();

    void insert(int wpm, const std::string& exercise);

    // Fastest-first list of every recorded entry (in-order traversal of the
    // BST, reversed, since the tree is ordered ascending by wpm).
    std::vector<Entry> fastestFirst() const;

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;

private:
    struct Node {
        Entry entry;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(Entry e) : entry(std::move(e)) {}
    };

    void insertNode(std::unique_ptr<Node>& node, Entry entry);
    void inOrder(const Node* node, std::vector<Entry>& out) const;

    std::unique_ptr<Node> root_;
};

#endif // GHOSTTYPIST_LEADERBOARD_HPP
