#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



#define MAX_NODES 1000
#define MAX_ID_LENGTH 256

typedef struct {
    char id[MAX_ID_LENGTH];
    int in_degree;
    int out_degree;
} Node;

typedef struct {
    Node nodes[MAX_NODES];
    int adjacency_matrix[MAX_NODES][MAX_NODES];
    int num_nodes;
    int num_edges;
} Graph;

// Function to initialize a graph
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