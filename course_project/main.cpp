#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <math.h> 

#define DB_SIZE 4000
#define PAGE_SIZE 20

// =============================================================
// STRUCTURES
// =============================================================

typedef struct {
    char fio[32];
    char street[18];
    short int house;
    short int flat;
    char date[10]; 
} Record;

typedef struct QNode {
    Record* data;
    struct QNode* next;
} QueueNode;

typedef struct TNode {
    Record* data;
    struct TNode *left;
    struct TNode *right;
} TreeNode;

// For Huffman
typedef struct {
    unsigned char symbol;
    double probability;
    char code[256];
    int length;
} SymbolInfo;

// =============================================================
// GLOBALS
// =============================================================

Record *database;
Record **index_arr;

QueueNode *q_head = NULL;
QueueNode *q_tail = NULL;

TreeNode *tree_root = NULL;

// =============================================================
// HELPER FUNCTIONS
// =============================================================

int get_day(char* date) { return (date[0] - '0') * 10 + (date[1] - '0'); }
int get_month(char* date) { return (date[3] - '0') * 10 + (date[4] - '0'); }

int get_full_year(char* date) {
    int y = (date[6] - '0') * 10 + (date[7] - '0');
    if (y < 25) return 2000 + y;
    else return 1900 + y;
}

int get_short_year(char* date) {
    return (date[6] - '0') * 10 + (date[7] - '0');
}

// =============================================================
// DATABASE IO
// =============================================================

void read_database() {
    FILE *fp = fopen("testBase4.dat", "rb");
    if (!fp) {
        printf("Error: File testBase4.dat not found!\n");
        exit(1);
    }
    database = (Record*)malloc(DB_SIZE * sizeof(Record));
    fread(database, sizeof(Record), DB_SIZE, fp);
    fclose(fp);

    index_arr = (Record**)malloc(DB_SIZE * sizeof(Record*));
    for (int i = 0; i < DB_SIZE; i++) index_arr[i] = &database[i];
}

void print_header() {
    printf("\n%-4s %-32s %-18s %-4s %-4s %-10s\n", "No", "FIO", "Street", "Dom", "Kv", "Date");
    printf("--------------------------------------------------------------------------------\n");
}

void print_record(Record *rec, int i) {
    // i is passed as 0-based index, so we print i+1
    printf("%-4d %-32.32s %-18.18s %-4d %-4d %-10.10s\n", 
           i + 1, rec->fio, rec->street, rec->house, rec->flat, rec->date);
}

int view_database(const char* title, int allow_sort_command) {
    int i = 0;
    while (i < DB_SIZE) {
        system("cls");
        printf("=== %s ===\n", title);
        print_header();
        int limit = i + PAGE_SIZE;
        if (limit > DB_SIZE) limit = DB_SIZE;
        for (int j = i; j < limit; j++) print_record(index_arr[j], j);
        i = limit;
        
        printf("\nCommands: [Enter] Next Page, [Esc] Main Menu");
        if (allow_sort_command) printf(", [S] SORT NOW");
        printf(": ");

        int ch = getch();
        if (ch == 27) return 0; 
        if (allow_sort_command && (ch == 's' || ch == 'S')) return 1; 
    }
    printf("\nEnd of list. Press any key...");
    getch();
    return 0;
}

// =============================================================
// HEAP SORT
// =============================================================

int compare_records(Record *a, Record *b) {
    int ya = get_full_year(a->date);
    int yb = get_full_year(b->date);
    if (ya != yb) return ya - yb;
    
    int ma = get_month(a->date);
    int mb = get_month(b->date);
    if (ma != mb) return ma - mb;
    
    int da = get_day(a->date);
    int db = get_day(b->date);
    if (da != db) return da - db;
    
    return strcmp(a->street, b->street);
}

void sift(Record **arr, int L, int R) {
    int i = L;
    int j = 2 * i + 1;
    Record *x = arr[L];
    while (j <= R) {
        if (j < R && compare_records(arr[j + 1], arr[j]) > 0) j++;
        if (compare_records(x, arr[j]) >= 0) break;
        arr[i] = arr[j];
        i = j;
        j = 2 * i + 1;
    }
    arr[i] = x;
}

void heap_sort() {
    int L = DB_SIZE / 2 - 1;
    while (L >= 0) { sift(index_arr, L, DB_SIZE - 1); L--; } // build max heap
    int R = DB_SIZE - 1;
    while (R > 0) {
        Record *temp = index_arr[0];
        index_arr[0] = index_arr[R];
        index_arr[R] = temp;
        R--;
        sift(index_arr, 0, R); // sift
    }
}

// =============================================================
// BINARY SEARCH & QUEUE
// =============================================================

void clear_queue() {
    QueueNode *current = q_head;
    while (current != NULL) {
        QueueNode *temp = current;
        current = current->next;
        free(temp);
    }
    q_head = NULL;
    q_tail = NULL;
}

void enqueue(Record* rec) {
    QueueNode *new_node = (QueueNode*)malloc(sizeof(QueueNode));
    new_node->data = rec;
    new_node->next = NULL;
    if (q_tail == NULL) { q_head = new_node; q_tail = new_node; } 
    else { q_tail->next = new_node; q_tail = new_node; }
}

int queue_length() {
    int count = 0;
    QueueNode *curr = q_head;
    while(curr) { count++; curr = curr->next; }
    return count;
}

void binary_search_and_queue() {
    int key;
    system("cls");
    printf("=== STEP: BINARY SEARCH ===\n");
    printf("Enter Year (2 digits, e.g. 96): ");
    if (scanf("%d", &key) != 1) {
        while(getchar()!='\n'); return;
    }
    while(getchar()!='\n');

    int L = 0, R = DB_SIZE - 1;
    while (L < R) {
        int m = (L + R) / 2;
        int current = get_short_year(index_arr[m]->date);
        if (current < key) L = m + 1;
        else R = m;
    }

    if (get_short_year(index_arr[R]->date) == key) {
        printf("Records found. Building Queue...\n");
        clear_queue();
        int i = R;
        while (i < DB_SIZE && get_short_year(index_arr[i]->date) == key) {
            enqueue(index_arr[i]);
            i++;
        }
        
        printf("\n--- Queue Contents ---\n");
        QueueNode *curr = q_head;
        int idx = 1;
        print_header();
        while(curr) {
            print_record(curr->data, idx++ - 1);
            curr = curr->next;
        }
    } else {
        printf("No records found for year %02d.\n", key);
        clear_queue();
    }
    printf("\nPress any key to continue...");
    getch();
}

// =============================================================
// OPTIMAL TREE (A2)
// =============================================================

void free_tree(TreeNode* node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

void sort_temp_array(Record** arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j]->flat > arr[j+1]->flat) {
                Record* temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

TreeNode* build_tree_A2(Record** arr, int L, int R) {
    if (L > R) return NULL;
    double wes = 0;
    for (int k = L; k <= R; k++) wes += 1.0; 

    double summa = 0;
    int i;
    for (i = L; i < R; i++) {
        if (summa < wes / 2.0 && (summa + 1.0) >= wes / 2.0) break; 
        summa += 1.0; 
    }
    
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->data = arr[i];
    node->left = build_tree_A2(arr, L, i - 1);
    node->right = build_tree_A2(arr, i + 1, R);
    return node;
}

// FIX 1: Added counter pointer to maintain "No" across recursion
void print_tree_recursive(TreeNode* node, int *counter) {
    if (node == NULL) return;
    print_tree_recursive(node->left, counter);
    
    // Print with the current counter value, then increment
    print_record(node->data, (*counter)); 
    (*counter)++;
    
    print_tree_recursive(node->right, counter);
}

// FIX 2: Recursive search that finds ALL duplicates
void search_tree_recursive(TreeNode* node, int key, int *found_count) {
    if (node == NULL) return;

    // Because duplicates can be on BOTH sides in A2 constructed trees (due to array split),
    // we must check equality first, and conditional logic for others.
    
    if (key < node->data->flat) {
        search_tree_recursive(node->left, key, found_count);
    } 
    else if (key > node->data->flat) {
        search_tree_recursive(node->right, key, found_count);
    } 
    else {
        // MATCH FOUND
        print_record(node->data, (*found_count));
        (*found_count)++;

        // Continue searching BOTH subtrees for potential duplicates
        search_tree_recursive(node->left, key, found_count);
        search_tree_recursive(node->right, key, found_count);
    }
}

void tree_logic() {
    if (q_head == NULL) {
        printf("\nQueue is empty. Cannot build tree.\nPress any key...");
        getch();
        return;
    }

    if (tree_root) { free_tree(tree_root); tree_root = NULL; }

    int n = queue_length();
    Record** temp_arr = (Record**)malloc(n * sizeof(Record*));
    QueueNode* curr = q_head;
    for(int k=0; k<n; k++) { temp_arr[k] = curr->data; curr = curr->next; }

    sort_temp_array(temp_arr, n);
    tree_root = build_tree_A2(temp_arr, 0, n - 1);
    free(temp_arr);

    system("cls");
    printf("=== STEP: OPTIMAL TREE (A2) ===\n");
    printf("Tree built by Flat Number.\n\n");
    print_header();
    
    int counter = 0; // Initialize counter for display
    print_tree_recursive(tree_root, &counter);
    
    // Search in Tree
    int key_flat;
    printf("\n>>> Tree Search <<<\n");
    printf("Enter Flat Number to search: ");
    if (scanf("%d", &key_flat) == 1) {
        printf("Searching...\n");
        print_header(); // Print header for results
        
        int found_count = 0;
        search_tree_recursive(tree_root, key_flat, &found_count);
        
        if (found_count == 0) printf("Flat %d not found in the tree.\n", key_flat);
        else printf("\nTotal found: %d\n", found_count);
    }
    while(getchar()!='\n'); 
    printf("\nPress any key to continue...");
    getch();
}

// =============================================================
// HUFFMAN CODING
// =============================================================

char C_matrix[256][256];
int L_arr[256];

void Up(int n, double q, double *P, int *j_out) {
    int i;
    int j = 0; 
    for (i = n - 2; i >= 1; i--) {
        if (P[i-1] <= q) {
            P[i] = P[i-1];
        } else {
            j = i;
            goto found;
        }
    }
    j = i; 
found:
    P[j] = q;
    *j_out = j;
}

void Down(int n, int j) {
    char S[256];
    strcpy(S, C_matrix[j]);
    
    for (int i = j; i < n - 1; i++) {
        strcpy(C_matrix[i], C_matrix[i+1]);
        L_arr[i] = L_arr[i+1];
    }

    strcpy(C_matrix[n-2], S);
    strcpy(C_matrix[n-1], S);

    strcat(C_matrix[n-2], "0");
    strcat(C_matrix[n-1], "1");

    L_arr[n-2] = strlen(C_matrix[n-2]);
    L_arr[n-1] = strlen(C_matrix[n-1]);
}

void Huffman(int n, double *P) {
    if (n == 2) {
        strcpy(C_matrix[0], "0"); L_arr[0] = 1;
        strcpy(C_matrix[1], "1"); L_arr[1] = 1;
    } else {
        double q = P[n-2] + P[n-1];
        int j;
        Up(n, q, P, &j);
        Huffman(n - 1, P);
        Down(n, j);
    }
}

int compare_symbols(const void *a, const void *b) {
    SymbolInfo *sa = (SymbolInfo*)a;
    SymbolInfo *sb = (SymbolInfo*)b;
    if (sb->probability > sa->probability) return 1;
    if (sb->probability < sa->probability) return -1;
    return 0;
}

void huffman_coding() {
    system("cls");
    printf("=== STEP: HUFFMAN CODING ===\n");
    printf("Calculating probabilities...\n");

    long counts[256] = {0};
    long total_bytes = 0;
    
    unsigned char *ptr = (unsigned char*)database;
    size_t total_size = DB_SIZE * sizeof(Record);
    for (size_t i = 0; i < total_size; i++) {
        counts[ptr[i]]++;
        total_bytes++;
    }

    SymbolInfo symbols[256];
    int n = 0;
    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            symbols[n].symbol = (unsigned char)i;
            symbols[n].probability = (double)counts[i] / total_bytes;
            n++;
        }
    }

    qsort(symbols, n, sizeof(SymbolInfo), compare_symbols);

    double *P = (double*)malloc(n * sizeof(double));
    for(int i=0; i<n; i++) P[i] = symbols[i].probability;

    Huffman(n, P);

    printf("\n%-10s %-12s %-20s %-10s\n", "Symbol", "Prob(Pi)", "Code", "Length(Li)");
    printf("-------------------------------------------------------\n");

    double avg_len = 0;
    double entropy = 0;

    for (int i = 0; i < n; i++) {
        strcpy(symbols[i].code, C_matrix[i]);
        symbols[i].length = L_arr[i];

        char display_char = symbols[i].symbol;
        if (display_char < 32) display_char = '.'; 
        
        if (i < 15 || i > n - 5) { 
            printf("'%c' (0x%02X)  %.6f     %-20s %d\n", 
                display_char, symbols[i].symbol, symbols[i].probability, 
                symbols[i].code, symbols[i].length);
        } else if (i == 15) {
            printf("... (skipping middle rows) ...\n");
        }

        avg_len += symbols[i].probability * symbols[i].length;
        if (symbols[i].probability > 0) {
            entropy -= symbols[i].probability * log2(symbols[i].probability);
        }
    }

    printf("-------------------------------------------------------\n");
    printf("Average Code Length (L_avg): %.4f\n", avg_len);
    printf("Entropy (H):                 %.4f\n", entropy);
    printf("Comparison: L_avg %s H + 1\n", (avg_len < entropy + 1) ? "<" : ">=");

    free(P);
    printf("\nPress any key to finish...");
    getch();
}

// =============================================================
// MAIN
// =============================================================

int main() {
    SetConsoleCP(866);
    SetConsoleOutputCP(866);

    read_database();

    int sort_requested = view_database("Unsorted Database (Load)", 1);
    
    if (sort_requested) {
        printf("\nSorting (Heap Sort)... Please wait.\n");
        heap_sort();
        printf("Sorted! Press any key to view.\n");
        getch();

        view_database("Sorted Database (Date+Street)", 0);

        binary_search_and_queue();

        tree_logic();

        huffman_coding();
    } else {
        printf("\nExited without sorting.\n");
    }

    if (database) free(database);
    if (index_arr) free(index_arr);
    clear_queue();
    free_tree(tree_root);

    return 0;
}