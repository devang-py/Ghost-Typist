#include "terminal.hpp"
#include "bracket_stack.hpp"
#include "trie.hpp"
#include "leaderboard.hpp"
#include "weakspot_graph.hpp"
#include "ghost.hpp"
#include "exercises.hpp"

#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <cerrno>
#include <algorithm>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

using Clock = std::chrono::steady_clock;

namespace {

std::string storagePrefix;

bool makeDir(const std::string& path) {
#ifdef _WIN32
    int rc = _mkdir(path.c_str());
#else
    int rc = mkdir(path.c_str(), 0755);
#endif
    return rc == 0 || errno == EEXIST;
}

void initStorage() {
    if (makeDir("data")) {
        storagePrefix = "data/";
    } else {
        storagePrefix = "";
        std::cout << Terminal::yellow("warning: could not create 'data' folder, saving next to the program instead") << "\n";
    }
}

std::string ghostPathFor(const std::string& exerciseName) {
    return storagePrefix + exerciseName + ".ghost";
}

std::string leaderboardPath() { return storagePrefix + "leaderboard.txt"; }
std::string weakspotsPath()   { return storagePrefix + "weakspots.graph"; }

int countWords(const std::string& text) {
    std::istringstream iss(text);
    std::string word;
    int count = 0;
    while (iss >> word) count++;
    return count > 0 ? count : 1;
}

std::string renderTargetLine(const std::string& target, const std::string& typed, int ghostIndex) {
    std::string line;
    for (size_t i = 0; i < target.size(); ++i) {
        bool isNewline = target[i] == '\n';
        std::string ch(1, isNewline ? '|' : target[i]);
        if (static_cast<int>(i) == ghostIndex) {
            line += Terminal::dimHighlight(ch);
        } else if (i < typed.size()) {
            line += (typed[i] == target[i]) ? Terminal::green(ch) : Terminal::red(ch);
        } else {
            line += Terminal::grey(ch);
        }
        if (isNewline) line += "\n";
    }
    return line;
}

std::string lastWordOf(const std::string& typed) {
    size_t i = typed.size();
    while (i > 0 && isalpha(static_cast<unsigned char>(typed[i - 1]))) {
        --i;
    }
    return typed.substr(i);
}

struct AttemptResult {
    int wpm = 0;
    std::vector<KeystrokeEvent> events;
    std::vector<std::pair<char, char>> mistakes;
    bool completed = false;
};

AttemptResult runAttempt(Terminal& term, const Exercise& exercise, const GhostRun* ghost, const Trie& trie, bool isTrial) {
    AttemptResult result;
    std::string typed;
    auto startTime = Clock::now();
    bool started = false;

    std::queue<KeystrokeEvent> ghostQueue;
    bool ghostActive = (ghost != nullptr && !ghost->isEmpty());
    if (ghostActive) ghostQueue = ghost->replayQueue();
    int ghostIndex = 0;

    Terminal::hideCursor();

    while (true) {
        long long elapsedMs = started
            ? std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime).count()
            : 0;

        while (ghostActive && !ghostQueue.empty() && ghostQueue.front().elapsedMs <= elapsedMs) {
            ghostIndex = ghostQueue.front().typedLength;
            ghostQueue.pop();
        }
        if (!ghostActive) ghostIndex = -1;

        std::cout << "\x1b[2J\x1b[H";
        std::cout << "ghosttypist $ "
                  << (isTrial ? "TRIAL RUN -- this sets your ghost" : "racing your best ghost")
                  << "\n\n";
        std::cout << "type both paragraphs, press Enter where you see | (ends automatically at the last character, ESC to quit):\n\n";
        std::cout << renderTargetLine(exercise.text, typed, ghostIndex) << "\n\n";

        std::string bracketWarning = BracketStack::checkLive(typed);
        if (!bracketWarning.empty()) {
            std::cout << Terminal::yellow("warning: " + bracketWarning) << "\n";
        } else {
            std::cout << "\n";
        }

        std::string word = lastWordOf(typed);
        auto suggestions = trie.suggest(word, 1);
        if (!word.empty() && !suggestions.empty()) {
            std::cout << Terminal::grey("suggest: " + word + "[" + suggestions[0] + "]  (tab to accept)") << "\n";
        } else {
            suggestions.clear();
            std::cout << "\n";
        }

        double elapsedSec = elapsedMs / 1000.0;
        std::cout << "\ntime: " << elapsedSec << "s   mistakes: " << result.mistakes.size();
        if (ghostActive) {
            int gap = static_cast<int>(typed.size()) - ghostIndex;
            if (gap > 0) std::cout << "   ghost: " << gap << " chars behind you";
            else if (gap < 0) std::cout << "   ghost: " << (-gap) << " chars ahead of you";
            else std::cout << "   ghost: neck and neck";
        }
        std::cout << "\n\n> " << typed << std::flush;

        if (typed.size() >= exercise.text.size()) {
            result.completed = true;
            double finalElapsed = std::max(elapsedSec, 0.01);
            result.wpm = static_cast<int>(std::lround((countWords(exercise.text) / finalElapsed) * 60.0));
            break;
        }

        int key = -1;
        auto pollStart = Clock::now();
        while (std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - pollStart).count() < 16) {
            key = term.readKeyNonBlocking();
            if (key != -1) break;
        }

        if (key == -1) continue;

        if (!started) {
            started = true;
            startTime = Clock::now();
        }

        if (key == 27) {
            break;
        } else if (key == 127 || key == 8) {
            if (!typed.empty()) typed.pop_back();
        } else if (key == 9) {
            if (!suggestions.empty()) typed += suggestions[0];
        } else if (key == 13 || key == 10 || (key >= 32 && key <= 126)) {
            char c = (key == 13 || key == 10) ? '\n' : static_cast<char>(key);
            size_t pos = typed.size();
            typed.push_back(c);
            if (pos < exercise.text.size() && exercise.text[pos] != c && c != '\n' && exercise.text[pos] != '\n') {
                result.mistakes.emplace_back(exercise.text[pos], c);
            }
        }

        result.events.push_back(KeystrokeEvent{
            static_cast<int>(typed.size()),
            std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime).count()
        });
    }

    Terminal::showCursor();
    return result;
}

void showIntro(Terminal& term) {
    std::vector<std::string> paragraphs = exerciseParagraphs();
    std::cout << "\x1b[2J\x1b[H";
    std::cout << "=== GHOST TYPIST ===\n\n";
    std::cout << "Type the two paragraphs below as fast and as cleanly as you can.\n";
    std::cout << "Your first run becomes your ghost, and every run after that races it.\n\n";
    for (size_t i = 0; i < paragraphs.size(); ++i) {
        std::cout << "Paragraph " << (i + 1) << ":\n" << paragraphs[i] << "\n\n";
    }
    std::cout << "Press any key to begin..." << std::flush;
    term.readKeyBlocking();
}

}

int main() {
    initStorage();

    Trie trie;
    for (const auto& word : autocompleteVocabulary()) trie.insert(word);

    Leaderboard leaderboard;
    leaderboard.loadFromFile(leaderboardPath());

    WeakSpotGraph weakspots;
    weakspots.loadFromFile(weakspotsPath());

    const Exercise exercise = defaultExercise();

    Terminal term;
    showIntro(term);

    while (true) {
        GhostRun ghost;
        bool hasGhost = ghost.loadFromFile(ghostPathFor(exercise.name))
                        && !ghost.isEmpty()
                        && ghost.text() == exercise.text;
        bool isTrial = !hasGhost;

        AttemptResult attempt = runAttempt(term, exercise, hasGhost ? &ghost : nullptr, trie, isTrial);

        if (!attempt.completed) {
            std::cout << "\n\nQuit before finishing -- nothing was saved.\n";
            return 0;
        }

        for (const auto& m : attempt.mistakes) {
            weakspots.recordMistake(m.first, m.second);
        }
        if (!weakspots.saveToFile(weakspotsPath())) {
            std::cout << "\n" << Terminal::yellow("warning: could not save weak spots to " + weakspotsPath());
        }

        leaderboard.insert(attempt.wpm, exercise.name);
        if (!leaderboard.saveToFile(leaderboardPath())) {
            std::cout << "\n" << Terminal::yellow("warning: could not save leaderboard to " + leaderboardPath());
        }

        std::cout << "\n\n=== RESULT ===\n";
        std::cout << "wpm: " << attempt.wpm << "   mistakes: " << attempt.mistakes.size() << "\n";

        if (isTrial || attempt.wpm > ghost.wpm()) {
            GhostRun newGhost(exercise.text, attempt.events, attempt.wpm);
            std::string path = ghostPathFor(exercise.name);
            if (newGhost.saveToFile(path)) {
                std::cout << (isTrial ? "Trial complete -- this run is now your ghost.\n"
                                      : "New best! Ghost updated to " + std::to_string(attempt.wpm) + " wpm.\n");
            } else {
                std::cout << Terminal::red("Could not save your ghost to " + path + " -- it will not persist.") << "\n";
            }
        } else {
            std::cout << "Ghost stayed ahead -- best is still " << ghost.wpm() << " wpm.\n";
        }

        std::string pattern = weakspots.worstPattern();
        if (!pattern.empty()) {
            std::cout << "Weak spot: " << pattern << "\n";
        }

        std::cout << "\nLeaderboard (fastest first):\n";
        for (const auto& entry : leaderboard.fastestFirst()) {
            std::cout << "  " << entry.wpm << " wpm  -  " << entry.exercise << "\n";
        }

        std::cout << "\nPress R to race your ghost again, any other key to quit." << std::flush;
        int key = term.readKeyBlocking();
        if (key != 'r' && key != 'R') {
            std::cout << "\n";
            break;
        }
    }

    return 0;
}
