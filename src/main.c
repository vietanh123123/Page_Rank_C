#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h> // For isdigit
#include "utils.h"
#include "graph.h"

void print_helppage () {
    printf("Usage: ./pagerank [OPTIONS] ... [FILENAME]\n");
    printf("Perform pagerank computations for a given file in the DOT format\n\n");
    printf("  -h        Print a brief overview of the available command line parameters\n");
    printf("  -r N      Simulate N steps of the random surfer and output the result\n");
    printf("  -m N      Simulate N steps of the Markov chain and output the result\n");
    printf("  -s        Compute and print the statistics of the graph\n");
    printf("  -p P      Set the teleportation parameter p to P%%. (Default: P = 10)\n");
}

// Helper to check if a string is purely numeric
int is_numeric(const char *s) {
    if (s == NULL || *s == '\0' || isspace(*s)) return 0;
    char *p;
    strtol(s, &p, 10);
    return *p == '\0';
}


int main(int argc, char *const argv[]) {
    char option;
    char *filename = NULL;
    int s_flag = 0; // Flag for -s option
    int r_steps = -1; // Steps for random surfer (-1 means not specified)
    int m_steps = -1; // Steps for Markov chain (-1 means not specified)
    int p_percent = 10; // Default teleportation percentage
    double teleport_prob = 0.10; // Teleportation probability derived from p_percent

    // Input validation: Check if no arguments are provided
    if (argc == 1) {
         print_helppage();
         exit(0);
    }

    while ((option = getopt(argc, argv, "hr:m:sp:")) != -1) {
        switch (option) {
            case 'h':
                print_helppage();
                exit(0);
            case 's':
                s_flag = 1; // Set the flag when -s is encountered
                break;
            case 'r':
                if (!is_numeric(optarg) || (r_steps = atoi(optarg)) < 0) {
                    fprintf(stderr, "Error: Invalid number of steps N for -r option: '%s'. N must be a non-negative integer.\n", optarg);
                    exit(1);
                }
                break;
            case 'm':
                 if (!is_numeric(optarg) || (m_steps = atoi(optarg)) < 0) {
                    fprintf(stderr, "Error: Invalid number of steps N for -m option: '%s'. N must be a non-negative integer.\n", optarg);
                    exit(1);
                }
                break;
            case 'p':
                 if (!is_numeric(optarg) || (p_percent = atoi(optarg)) < 0 || p_percent > 100) {
                    fprintf(stderr, "Error: Invalid percentage P for -p option: '%s'. P must be between 0 and 100.\n", optarg);
                    exit(1);
                }
                teleport_prob = (double)p_percent / 100.0;
                break;
            default: // Handles unknown options or missing arguments for options
                fprintf(stderr, "Usage: %s [-h] [-r N] [-m N] [-s] [-p P] [FILENAME]\n", argv[0]);
                exit(1);
        }
    }

    // Get the filename from the remaining arguments
    if (optind < argc) {
        filename = argv[optind];
        // Optional: Check if more than one filename is provided
        if (optind + 1 < argc) {
            fprintf(stderr, "Error: Too many file names provided.\n");
            fprintf(stderr, "Usage: %s [-h] [-r N] [-m N] [-s] [-p P] [FILENAME]\n", argv[0]);
            exit(1);
        }
    } else {
        // Filename is required unless only -h was used (which exits)
         fprintf(stderr, "Error: No input file provided.\n");
         fprintf(stderr, "Usage: %s [-h] [-r N] [-m N] [-s] [-p P] [FILENAME]\n", argv[0]);
         exit(1);
    }

    // --- Action based on flags ---

    // Check for mutually exclusive options or specific combinations if needed
    // For example, maybe -r and -m shouldn't run together? (Current code allows it)
    if (r_steps >= 0 && m_steps >= 0) {
         fprintf(stderr, "Warning: Both -r and -m specified. Running both simulations.\n");
         // Or exit: fprintf(stderr, "Error: Cannot specify both -r and -m options.\n"); exit(1);
    }
    if (s_flag && (r_steps >= 0 || m_steps >= 0)) {
         fprintf(stderr, "Warning: -s specified with -r or -m. Running statistics first, then simulation(s).\n");
         // Or exit: fprintf(stderr, "Error: Cannot specify -s with -r or -m options.\n"); exit(1);
    }


    // Initialize graph common to multiple options
    Graph graph;
    init_graph(&graph);
    parse_dot_file(&graph, filename); // Assume parse handles file errors

    // Handle -s
    if (s_flag) {
        print_graph_stats(&graph);
        // Decide if -s should exit or continue to other operations
        // Based on common usage, -s usually just prints stats and exits.
        // If you want it to run *before* simulations, remove the exit(0).
        exit(0);
    }

    // Check if graph has nodes before simulations
    if (graph.num_nodes == 0 && (r_steps > 0 || m_steps > 0)) {
        fprintf(stderr, "Warning: Input graph has no nodes. Cannot run simulations.\n");
        // Decide whether to exit or just print empty results later
        if (r_steps > 0) printf("\nRandom Surfer Results (N=%d, p=%.2f):\n(No nodes)\n", r_steps, teleport_prob);
        if (m_steps > 0) printf("\nMarkov Chain Results (N=%d, p=%.2f):\n(No nodes)\n", m_steps, teleport_prob);
        exit(0); // Exit if no nodes for simulations
    }


    // Handle -r (Random Surfer)
    if (r_steps >= 0) {
         if (graph.num_nodes > 0) {
            printf("\nRunning Random Surfer Simulation (N=%d, p=%.2f)...\n", r_steps, teleport_prob);
            rand_init(); // Initialize random seed before simulation
            simulate_random_surfer(&graph, r_steps, teleport_prob);
         } else if (r_steps == 0) {
              printf("\nRandom Surfer Results (N=0, p=%.2f):\n(No steps performed)\n", teleport_prob);
         }
    }

    // Handle -m (Markov Chain)
    if (m_steps >= 0) {
         if (graph.num_nodes > 0) {
             printf("\nRunning Markov Chain Simulation (N=%d, p=%.2f)...\n", m_steps, teleport_prob);
             simulate_markov_chain(&graph, m_steps, teleport_prob);
         } else if (m_steps == 0) {
             printf("\nMarkov Chain Results (N=0, p=%.2f):\n", teleport_prob);
              // Print initial uniform distribution if steps = 0
             double initial_rank = 1.0 / graph.num_nodes; // Should not happen due to check above, but for completeness
             if (graph.num_nodes > 0) initial_rank = 1.0 / graph.num_nodes; else initial_rank = 0; // Avoid division by zero

             // Need node IDs to print correctly, maybe call a helper
             NodeRank *results = malloc(graph.num_nodes * sizeof(NodeRank));
             if (!results) { perror("Failed to allocate memory for results"); exit(1); }
             for (int i = 0; i < graph.num_nodes; ++i) {
                 strncpy(results[i].id, graph.nodes[i].id, MAX_ID_LENGTH - 1);
                 results[i].id[MAX_ID_LENGTH - 1] = '\0';
                 results[i].rank = initial_rank;
             }
             // Need the comparison function here too
             // int compare_node_ranks(const void *a, const void *b); // Declare or define
             // qsort(results, graph.num_nodes, sizeof(NodeRank), compare_node_ranks);
              for (int i = 0; i < graph.num_nodes; ++i) {
                    printf("- %s: %.6f\n", results[i].id, results[i].rank);
              }
              free(results);
         }
    }

    // Note: No need to free graph resources here if using stack allocation for Graph struct
    // and static arrays within it. If you used malloc inside graph functions, add cleanup.
    exit(0);
}