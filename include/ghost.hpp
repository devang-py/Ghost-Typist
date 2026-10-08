#ifndef GHOSTTYPIST_GHOST_HPP
#define GHOSTTYPIST_GHOST_HPP

#include <string>
#include <vector>
#include <queue>
#include <cstdint>

// ---------------------------------------------------------------------------
// GhostRun: one recorded attempt at an exercise.
//
// While the user types, every keystroke is pushed as a {typedLength,
// elapsedMs} event. That recorded sequence IS the ghost: replaying it later
// just means dequeuing events in order and, at each one's original
// timestamp, updating how many characters "the ghost" has typed so far.
// A std::queue is the natural fit here because playback is strictly
// first-in-first-out — you can't skip ahead or go back.
// ---------------------------------------------------------------------------
struct KeystrokeEvent {
    int typedLength;
    long long elapsedMs;
};

class GhostRun {
public:
    GhostRun() = default;
    GhostRun(std::string exerciseText, std::vector<KeystrokeEvent> events, int wpm);

    // Rebuilds a fresh queue from the stored events so the same GhostRun can
    // be replayed multiple times (once a std::queue is drained it's empty).
    std::queue<KeystrokeEvent> replayQueue() const;

    int wpm() const { return wpm_; }
    const std::string& text() const { return exerciseText_; }
    bool isEmpty() const { return events_.empty(); }

    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

private:
    std::string exerciseText_;
    std::vector<KeystrokeEvent> events_;
    int wpm_ = 0;
};

#endif // GHOSTTYPIST_GHOST_HPP
