# Ghost Typist

A terminal-only typing/coding speed trainer. No browser, no server — you race
a live replay of your own best attempt, right inside your terminal.

## Build

Requires a C++17 compiler (g++ or clang++) and `make`. Linux/macOS/WSL.

```
make
./bin/ghosttypist
```

On native Windows (cmd/PowerShell, not WSL), `terminal.cpp` has a Windows
code path (`_kbhit`/`_getch` + `ENABLE_VIRTUAL_TERMINAL_PROCESSING`), but it
hasn't been build-tested on an actual Windows machine — only the POSIX path
has been compiled and run here. If you build on Windows and hit issues with
raw input, that's the first place to look.

## How it works

1. Read the two paragraphs on the intro screen (edit them in
   `include/exercises.hpp`), then press any key to start.
2. **First attempt is always a trial** — there's no ghost yet, so whatever
   you type becomes your baseline, keystroke-by-keystroke.
3. **Every attempt after that races your best-ever run.** The ghost's
   current position shows as a highlighted block moving through the same
   line you're typing, plus a live "N chars ahead/behind" readout.
4. Your ghost only updates when you genuinely beat your previous best WPM —
   slower runs don't overwrite it.
5. Live feedback while you type:
   - Unmatched brackets flash a warning immediately (via `BracketStack`).
   - A tiny trie suggests completions for common tokens (Tab to accept).
6. After each run, your personal-best leaderboard and a "weak spot" pattern
   (characters you keep mistyping) print to the terminal.

## Where each data structure lives

| Data structure | File | What it does |
|---|---|---|
| Stack | `bracket_stack.{hpp,cpp}` | Live bracket/paren matching as you type |
| Queue | `ghost.{hpp,cpp}` | Records & replays keystroke timing — this *is* the ghost |
| Trie | `trie.{hpp,cpp}` | Prefix-based autocomplete suggestions |
| Binary Search Tree | `leaderboard.{hpp,cpp}` | Keeps personal bests sorted by WPM |
| Graph | `weakspot_graph.{hpp,cpp}` | Tracks which characters you confuse, across exercises |

`src/main.cpp` is the orchestrator: it owns the raw-terminal render loop and
wires the five structures together.

## Persistence

Saved to `data/` in the folder you run the program from (created automatically):
- `data/paragraphs.ghost` — your best recorded run
- `data/leaderboard.txt` — WPM history
- `data/weakspots.graph` — accumulated mistake patterns

## Known limitation (honest note)

The ESC-to-quit and raw keystroke capture were compiled and verified to
build cleanly, but could only be smoke-tested here via piped, non-interactive
input — which isn't representative of a real terminal session (a real TTY
holds keystrokes back until Enter is pressed for the menu, then hands over
clean raw control; a pipe doesn't). You'll want to actually run this
interactively yourself to confirm the live typing feel works the way you
expect — if the ghost timing or redraw feels off, the render loop in
`runAttempt()` (src/main.cpp) is the place to tune it.
