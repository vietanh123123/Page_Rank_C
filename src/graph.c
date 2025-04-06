#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "graph.h"


void init_graph(Graph* graph) {
    graph->num_nodes = 0;
    graph->num_edges = 0;
    memset(graph->adjacency_matrix, 0, sizeof(graph->adjacency_matrix));
}

// Function to find a node index by ID
int find_node_index(Graph* graph, const char* id) {
    for (int i = 0; i < graph->num_nodes; i++) {
        if (strcmp(graph->nodes[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to add a node to the graph
int add_node(Graph* graph, const char* id) {
    int index = find_node_index(graph, id);
    if (index != -1) {
        return index; // Node already exists
    }
    if (graph->num_nodes >= MAX_NODES) {
        fprintf(stderr, "Error: Maximum number of nodes reached\n");
        exit(1);
    }
    strncpy(graph->nodes[graph->num_nodes].id, id, MAX_ID_LENGTH - 1);
    graph->nodes[graph->num_nodes].id[MAX_ID_LENGTH - 1] = '\0';
    graph->nodes[graph->num_nodes].in_degree = 0;
    graph->nodes[graph->num_nodes].out_degree = 0;
    return graph->num_nodes++;
}

// Function to add an edge between two nodes
void add_edge(Graph* graph, const char* source_id, const char* target_id) {
    int source_index = add_node(graph, source_id);
    int target_index = add_node(graph, target_id);

    if (!graph->adjacency_matrix[source_index][target_index]) {
        graph->adjacency_matrix[source_index][target_index] = 1;
        graph->nodes[source_index].out_degree++;
        graph->nodes[target_index].in_degree++;
        graph->num_edges++;
    }
}

// Function to parse a DOT file and build the graph
void parse_dot_file(Graph* graph, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1); // Or return an error code
    }

    char line[1024];
    // Validate the header line
    if (!fgets(line, sizeof(line), file)) {
        fprintf(stderr, "Error: File is empty or could not be read: %s\n", filename);
        fclose(file);
        exit(1); // Or return an error code
    }

    char graph_id[MAX_ID_LENGTH];
    // Use width specifier for safety
    if (sscanf(line, " digraph %255s {", graph_id) != 1) { // Check for leading whitespace too
        // Try without space in case format is strict 'digraph G {'
        if (sscanf(line, "digraph %255s {", graph_id) != 1) {
             fprintf(stderr, "Error: File '%s' must start with 'digraph <identifier> {'\n", filename);
             fclose(file);
             exit(1); // Or return an error code
        }
    }


    // Validate the graph identifier (looks okay)
    if (!isalpha(graph_id[0])) {
        fprintf(stderr, "Error: Graph identifier '%s' in '%s' must start with a letter\n", graph_id, filename);
        fclose(file);
        exit(1);
    }
    for (int i = 1; graph_id[i] != '\0'; i++) {
        if (!isalnum(graph_id[i]) && graph_id[i] != '_') { // Allow underscore? Common practice. Adjust if needed.
            fprintf(stderr, "Error: Graph identifier '%s' in '%s' must contain only letters, numbers, or underscores\n", graph_id, filename);
            fclose(file);
            exit(1);
        }
    }


    // Parse edges
    while (fgets(line, sizeof(line), file)) {
        // Trim leading/trailing whitespace (optional but good practice)
        char* trimmed_line = line;
        while (isspace((unsigned char)*trimmed_line)) trimmed_line++;
        char* end = trimmed_line + strlen(trimmed_line) - 1;
        while(end > trimmed_line && isspace((unsigned char)*end)) end--;
        *(end + 1) = 0;


        // Check for the closing brace
        if (strcmp(trimmed_line, "}") == 0) {
            break;
        }

        // Skip empty or comment lines (optional)
        if (strlen(trimmed_line) == 0 || trimmed_line[0] == '#') { // Example comment handling
            continue;
        }


        char source_id[MAX_ID_LENGTH], target_id[MAX_ID_LENGTH];
        // Use width specifiers for safety. Be flexible with spaces around -> and ;
        if (sscanf(trimmed_line, "%255s -> %255s ;", source_id, target_id) != 2 &&
            sscanf(trimmed_line, "%255s->%255s;", source_id, target_id) != 2 ) { // Add variations if needed
            fprintf(stderr, "Error: Invalid edge format in file '%s': %s\n", filename, line); // Show original line
            fclose(file);
            exit(1);
        }

        // Validate source and target identifiers (looks okay, maybe allow '_')
         if (!isalpha(source_id[0])) { /* ... error ... */ exit(1); }
         for (int i = 1; source_id[i] != '\0'; i++) {
            if (!isalnum(source_id[i]) && source_id[i] != '_') { /* ... error ... */ exit(1); }
         }
         if (!isalpha(target_id[0])) { /* ... error ... */ exit(1); }
         for (int i = 1; target_id[i] != '\0'; i++) {
            if (!isalnum(target_id[i]) && target_id[i] != '_') { /* ... error ... */ exit(1); }
         }


        // Add the edge to the graph
        add_edge(graph, source_id, target_id);
    }

    // Check if the file ended without a closing brace (optional)


    fclose(file);
}

// Function to print graph statistics
void print_graph_stats(Graph* graph) {
    printf("Graph Statistics:\n");
    printf("- Number of nodes: %d\n", graph->num_nodes);
    printf("- Number of edges: %d\n", graph->num_edges);

    if (graph->num_nodes == 0) {
        printf("- In-degree range: N/A\n");
        printf("- Out-degree range: N/A\n");
    } else {
        // Initialize with the first node's degrees
        int min_in_degree = graph->nodes[0].in_degree;
        int max_in_degree = graph->nodes[0].in_degree;
        int min_out_degree = graph->nodes[0].out_degree;
        int max_out_degree = graph->nodes[0].out_degree;

        // Iterate from the second node onwards
        for (int i = 1; i < graph->num_nodes; i++) {
            if (graph->nodes[i].in_degree < min_in_degree) {
                min_in_degree = graph->nodes[i].in_degree;
            }
            if (graph->nodes[i].in_degree > max_in_degree) {
                max_in_degree = graph->nodes[i].in_degree;
            }
            if (graph->nodes[i].out_degree < min_out_degree) {
                min_out_degree = graph->nodes[i].out_degree;
            }
            if (graph->nodes[i].out_degree > max_out_degree) {
                max_out_degree = graph->nodes[i].out_degree;
            }
        }
        printf("- In-degree range: %d-%d\n", min_in_degree, max_in_degree);
        printf("- Out-degree range: %d-%d\n", min_out_degree, max_out_degree);
    }
}


int compare_node_ranks(const void *a, const void *b) {
    NodeRank *rankA = (NodeRank *)a;
    NodeRank *rankB = (NodeRank *)b;
    return strcmp(rankA->id, rankB->id);
}

// --- Random Surfer Simulation ---
void simulate_random_surfer(Graph* graph, int steps, double teleport_prob) {
    if (graph->num_nodes == 0) {
        printf("Random Surfer Results (N=%d, p=%.2f):\n(No nodes in graph)\n", steps, teleport_prob);
        return;
    }
     if (steps <= 0) {
        printf("Random Surfer Results (N=%d, p=%.2f):\n(No simulation steps performed)\n", steps, teleport_prob);
        // Optionally print zero ranks for all nodes if N=0
        return;
    }


    int *visit_counts = calloc(graph->num_nodes, sizeof(int));
    if (!visit_counts) {
        perror("Failed to allocate memory for visit counts");
        exit(1);
    }

    // Start at a random node
    int current_node_index = randu(graph->num_nodes);
    // Note: The description often implies the *first* visit doesn't count towards rank,
    // or that N steps means N transitions. We'll count the node landed on *after* each step.

    for (int i = 0; i < steps; ++i) {
        // Decide whether to teleport or follow a link
        // randu(100) gives a number from 0 to 99.
        // teleport if randu(100) < p_percent
        int p_percent_int = (int)(teleport_prob * 100.0); // Integer percentage
        int should_teleport = (randu(100) < p_percent_int);

        int out_degree = graph->nodes[current_node_index].out_degree;

        if (should_teleport || out_degree == 0) {
            // Teleport (or jump from dangling node)
            current_node_index = randu(graph->num_nodes);
        } else {
            // Follow an outgoing link
            // Find neighbors
            int *neighbors = malloc(out_degree * sizeof(int));
            if (!neighbors) {
                 perror("Failed to allocate memory for neighbors");
                 free(visit_counts);
                 exit(1);
             }
            int neighbor_count = 0;
            for (int j = 0; j < graph->num_nodes; ++j) {
                if (graph->adjacency_matrix[current_node_index][j]) {
                    neighbors[neighbor_count++] = j;
                }
            }

            // Choose a random neighbor
            if (neighbor_count > 0) { // Should always be true if out_degree > 0
                 current_node_index = neighbors[randu(neighbor_count)];
            } else {
                // Should not happen if out_degree > 0, but as fallback: teleport
                 fprintf(stderr, "Warning: Node %s has out_degree %d but no neighbors found in matrix?\n",
                        graph->nodes[current_node_index].id, out_degree);
                 current_node_index = randu(graph->num_nodes);
            }
            free(neighbors);
        }
         // Increment visit count for the node *landed on*
        visit_counts[current_node_index]++;
    }

    // Prepare results for sorting and printing
    NodeRank *results = malloc(graph->num_nodes * sizeof(NodeRank));
     if (!results) {
        perror("Failed to allocate memory for results");
        free(visit_counts);
        exit(1);
    }

    printf("\nRandom Surfer Results (N=%d, p=%.2f):\n", steps, teleport_prob);
    for (int i = 0; i < graph->num_nodes; ++i) {
        strncpy(results[i].id, graph->nodes[i].id, MAX_ID_LENGTH - 1);
        results[i].id[MAX_ID_LENGTH - 1] = '\0';
        results[i].rank = (double)visit_counts[i] / steps;
    }

    // Sort results alphabetically by node ID
    qsort(results, graph->num_nodes, sizeof(NodeRank), compare_node_ranks);

    // Print sorted results
    for (int i = 0; i < graph->num_nodes; ++i) {
        printf("- %s: %.6f\n", results[i].id, results[i].rank);
    }

    free(visit_counts);
    free(results);
}


// --- Markov Chain Simulation ---
void simulate_markov_chain(Graph* graph, int steps, double teleport_prob) {
     if (graph->num_nodes == 0) {
        printf("Markov Chain Results (N=%d, p=%.2f):\n(No nodes in graph)\n", steps, teleport_prob);
        return;
    }

    // Allocate probability vectors
    double *current_prob = malloc(graph->num_nodes * sizeof(double));
    double *next_prob = malloc(graph->num_nodes * sizeof(double));
    if (!current_prob || !next_prob) {
         perror("Failed to allocate memory for probability vectors");
         free(current_prob); // free any that were allocated
         free(next_prob);
         exit(1);
    }

    // Initialize with uniform probability
    double initial_prob = 1.0 / graph->num_nodes;
    for (int i = 0; i < graph->num_nodes; ++i) {
        current_prob[i] = initial_prob;
    }

    // --- Run N iterations ---
    for (int k = 0; k < steps; ++k) {
        // Reset next_prob for this iteration
        memset(next_prob, 0, graph->num_nodes * sizeof(double));

        double dangle_sum = 0.0; // Sum of probabilities of being at a dangling node

        // Calculate contribution from links and identify dangling probability
        for (int i = 0; i < graph->num_nodes; ++i) {
            int out_degree = graph->nodes[i].out_degree;
            if (out_degree == 0) {
                dangle_sum += current_prob[i];
            } else {
                // Distribute (1-p) * prob[i] among neighbors
                double base_contrib = (1.0 - teleport_prob) * current_prob[i];
                for (int j = 0; j < graph->num_nodes; ++j) {
                    if (graph->adjacency_matrix[i][j]) {
                        // Add share of probability from node i
                        next_prob[j] += base_contrib / out_degree;
                    }
                }
            }
        }

        // Distribute dangling probability and teleport probability uniformly
        double uniform_contrib = (teleport_prob + dangle_sum) / graph->num_nodes;
        for (int j = 0; j < graph->num_nodes; ++j) {
             next_prob[j] += uniform_contrib;
        }

        // Optional: Check for convergence (sum should be close to 1)
        // double sum_check = 0;
        // for(int j=0; j<graph->num_nodes; ++j) sum_check += next_prob[j];
        // if (fabs(sum_check - 1.0) > 1e-5) { // Tolerance
        //     fprintf(stderr, "Warning: Probability sum after step %d is %f, expected 1.0\n", k + 1, sum_check);
        // }

        // Update current_prob for the next iteration
        memcpy(current_prob, next_prob, graph->num_nodes * sizeof(double));
    } // End of iterations loop

    // Prepare results for sorting and printing
    NodeRank *results = malloc(graph->num_nodes * sizeof(NodeRank));
     if (!results) {
        perror("Failed to allocate memory for results");
        free(current_prob);
        free(next_prob);
        exit(1);
    }

    printf("\nMarkov Chain Results (N=%d, p=%.2f):\n", steps, teleport_prob);
    for (int i = 0; i < graph->num_nodes; ++i) {
        strncpy(results[i].id, graph.nodes[i].id, MAX_ID_LENGTH - 1);
        results[i].id[MAX_ID_LENGTH - 1] = '\0';
        results[i].rank = current_prob[i]; // Final probability is the rank
    }

    // Sort results alphabetically by node ID
    qsort(results, graph->num_nodes, sizeof(NodeRank), compare_node_ranks);

    // Print sorted results
    for (int i = 0; i < graph->num_nodes; ++i) {
        printf("- %s: %.6f\n", results[i].id, results[i].rank);
    }

    // Cleanup
    free(current_prob);
    free(next_prob);
    free(results);
}
