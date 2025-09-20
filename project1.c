#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100
#define MAX_NAME_LEN 20
#define AMOUNT_THRESHOLD 10000.0



typedef struct Edge {
    int dest;
    double amount;
    struct Edge* next;
} Edge;

typedef struct {
    char name[MAX_NAME_LEN];
    Edge* head;
    double total_outgoing;
} Account;

typedef struct {
    Account accounts[MAX_ACCOUNTS];
    int size;
} Graph;


int find_account(Graph* g, const char* name) {
    int i;
    for(i = 0; i < g->size; i++) {
        if (strcmp(g->accounts[i].name, name) == 0) return i;
    }
    return -1;
}

int add_account(Graph* g, const char* name) {
    int idx = find_account(g, name);
    if (idx != -1) return idx;

    if (g->size >= MAX_ACCOUNTS) {
        printf("Too many accounts!\n");
        exit(1);
    }
    strcpy(g->accounts[g->size].name, name);
    g->accounts[g->size].head = NULL;
    g->accounts[g->size].total_outgoing = 0.0;
    g->size++;
    return g->size - 1;
}

void add_transaction(Graph* g, const char* sender, const char* receiver, double amount) {
    int u = add_account(g, sender);
    int v = add_account(g, receiver);

    Edge* e = (Edge*)malloc(sizeof(Edge));
    e->dest = v;
    e->amount = amount;
    e->next = g->accounts[u].head;
    g->accounts[u].head = e;

    g->accounts[u].total_outgoing += amount;
}



int dfs_cycle(Graph* g, int u, int* visited, int* stack) {
    visited[u] = 1;
    stack[u] = 1;

    Edge* e = g->accounts[u].head;
    while (e != NULL) {
        int v = e->dest;
        if (!visited[v] && dfs_cycle(g, v, visited, stack)) {
            return 1;
        } else if (stack[v]) {
            return 1; 
        }
        e = e->next;
    }

    stack[u] = 0;
    return 0;
}

int has_cycle(Graph* g) {
    int visited[MAX_ACCOUNTS] = {0};
    int stack[MAX_ACCOUNTS] = {0};
    int i;

    for (i = 0; i < g->size; i++) {
        if (!visited[i]) {
            if (dfs_cycle(g, i, visited, stack)) return 1;
        }
    }
    return 0;
}

// ---------------------------
// Risk Scoring
// ---------------------------

void compute_risk(Graph* g) {
    int i;
    printf("\n--- Risk Scores ---\n");
    for (i = 0; i < g->size; i++) {
        double score = 0.0;

        if (g->accounts[i].total_outgoing >= AMOUNT_THRESHOLD)
            score += 50.0;

        int visited[MAX_ACCOUNTS] = {0};
        int stack[MAX_ACCOUNTS] = {0};
        if (dfs_cycle(g, i, visited, stack))
            score += 50.0;

        printf("Account %-5s | Total Out: %.2f | Risk Score: %.2f\n",
               g->accounts[i].name, g->accounts[i].total_outgoing, score);
    }
}

// ---------------------------
// Free Memory
// ---------------------------

void free_graph(Graph* g) {
    int i;
    for (i = 0; i < g->size; i++) {
        Edge* e = g->accounts[i].head;
        while (e != NULL) {
            Edge* temp = e;
            e = e->next;
            free(temp);
        }
    }
}

// ---------------------------
// Main (Direct Execution)
// ---------------------------

int main() {
    Graph g;
    g.size = 0;
    int i;

    // Preloaded sample transactions
    add_transaction(&g, "A", "B", 5000.0);
    add_transaction(&g, "B", "C", 4500.0);
    add_transaction(&g, "C", "A", 4000.0); // cycle A->B->C->A

    add_transaction(&g, "D", "E", 20000.0);
    add_transaction(&g, "E", "F", 18000.0);
    add_transaction(&g, "F", "D", 15000.0); // cycle D->E->F->D

    add_transaction(&g, "M", "N", 15000.0);
    add_transaction(&g, "M", "O", 12000.0);

    printf("Accounts in system: %d\n", g.size);
    for (i = 0; i < g.size; i++) {
        printf("Account %-5s | Total Outgoing: %.2f\n",
               g.accounts[i].name, g.accounts[i].total_outgoing);
    }

    if (has_cycle(&g))
        printf("\n? Suspicious cycle detected!\n");
    else
        printf("\nNo cycles detected.\n");

    compute_risk(&g);

    free_graph(&g);

    printf("\nProgram finished successfully!\n");
    return 0;
}

