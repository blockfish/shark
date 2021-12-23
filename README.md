# Blockfish

This repository is the monorepo for the new version of blockfish + related tools such as [TBP](https://github.com/tetris-bot-protocol/tbp-spec/) implementation and interactive webpage frontend.

This codebase is still not officially released, but will eventually replace the Rust blockfish repository.

# "Shark"

Shark is the C++ rewrite of Blockfish. The heuristic functions are the same, but I am currently experimenting with the pathfinding algorithm.

Shark exposes a TBP interface and compiles to both a native CLI as well as Web Worker via emscripten.

# "Tank"

Tank is the web frontend application that allows you to interact with Blockfish using a
fumen-like system. In the future I hope to add additional tools like replay analysis
