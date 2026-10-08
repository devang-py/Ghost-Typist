#ifndef GHOSTTYPIST_EXERCISES_HPP
#define GHOSTTYPIST_EXERCISES_HPP

#include <string>
#include <vector>

struct Exercise {
    std::string name;
    std::string text;
};

inline std::vector<std::string> exerciseParagraphs() {
    return {
        "A terminal is a text-only window into your computer, where colors and cursor moves are controlled by escape sequences.",
        "Typing fast is about rhythm, not raw speed, so keep your eyes ahead and slow down slightly after any mistake."
    };
}

inline Exercise defaultExercise() {
    std::vector<std::string> paragraphs = exerciseParagraphs();
    std::string text;
    for (size_t i = 0; i < paragraphs.size(); ++i) {
        if (i > 0) text += '\n';
        text += paragraphs[i];
    }
    return {"paragraphs", text};
}

inline std::vector<std::string> autocompleteVocabulary() {
    return {
        "terminal", "text", "window", "computer", "character", "screen",
        "special", "escape", "sequence", "color", "cursor", "line",
        "typing", "rhythm", "speed", "fingers", "mistake", "accuracy",
        "habit", "keep", "slow", "build"
    };
}

#endif
