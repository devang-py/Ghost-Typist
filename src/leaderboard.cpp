#include "leaderboard.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

Leaderboard::Leaderboard() : root_(nullptr) {}

void Leaderboard::insertNode(std::unique_ptr<Node>& node, Entry entry) {
    if (!node) {
        node = std::make_unique<Node>(std::move(entry));
        return;
    }
    if (entry.wpm < node->entry.wpm) {
        insertNode(node->left, std::move(entry));
    } else {
        insertNode(node->right, std::move(entry));
    }
}

void Leaderboard::insert(int wpm, const std::string& exercise) {
    insertNode(root_, Entry{wpm, exercise});
}

void Leaderboard::inOrder(const Node* node, std::vector<Entry>& out) const {
    if (!node) return;
    inOrder(node->left.get(), out);
    out.push_back(node->entry);
    inOrder(node->right.get(), out);
}

std::vector<Leaderboard::Entry> Leaderboard::fastestFirst() const {
    std::vector<Entry> ascending;
    inOrder(root_.get(), ascending);
    std::reverse(ascending.begin(), ascending.end());
    return ascending;
}

bool Leaderboard::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string wpmStr, exercise;
        if (std::getline(iss, wpmStr, ',') && std::getline(iss, exercise)) {
            try {
                insert(std::stoi(wpmStr), exercise);
            } catch (...) {
                // skip malformed line rather than crashing on a corrupt file
                continue;
            }
        }
    }
    return true;
}

bool Leaderboard::saveToFile(const std::string& path) const {
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) return false;

    // Save in fastest-first order; order doesn't matter for correctness
    // since loadFromFile rebuilds the tree from scratch, but this makes the
    // raw file human-readable too.
    for (const auto& entry : fastestFirst()) {
        out << entry.wpm << "," << entry.exercise << "\n";
    }
    return true;
}
