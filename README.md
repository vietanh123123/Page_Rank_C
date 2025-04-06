This project implements the PageRank algorithm using two different simulation methods: the Random Surfer model and the iterative Markov Chain approach. It takes a graph definition in the standard DOT file format as input and calculates the PageRank scores for each node.

## Features

*   **DOT File Parsing:** Reads directed graphs specified in the DOT format.
*   **Graph Statistics:** Calculates and displays basic graph statistics (number of nodes/edges, min/max in/out degrees) using the `-s` option.
*   **Random Surfer Simulation:** Simulates the Random Surfer model for a specified number of steps (`-r N`) to estimate PageRank scores.
*   **Markov Chain Simulation:** Calculates PageRank scores iteratively using the power iteration method on the corresponding Markov chain for a specified number of steps (`-m N`).
*   **Configurable Teleportation:** Allows setting the teleportation probability (damping factor `1-p`) via the `-p P` option, where `P` is the percentage chance of teleporting (default is 10%).
*   **Command-line Interface:** Provides a standard command-line interface using `getopt`.
*   **Sorted Output:** PageRank results from both simulation methods are printed sorted alphabetically by node ID.


Option	Argument	Description
-h		Print a help message summarizing the options and exit.
-s		Compute and print statistics about the input graph and exit.
-r N	N	Simulate N steps of the Random Surfer model. N must be >= 0.
-m N	N	Simulate N steps (iterations) of the Markov Chain model. N must be >= 0.
-p P	P	Set the teleportation probability parameter p to P%. P must be 0-100. (Default: 10).
Arguments:
FILENAME: The path to the input graph file in DOT format. This argument is required unless only -h is specified.
