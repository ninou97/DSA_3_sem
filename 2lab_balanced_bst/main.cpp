#include <stdio.h>
#include <stdlib.h>

#define N 100

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

// Функция создания узла
Node* createNode(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

// Обход слева направо (Inorder)
void inorder(Node* root) {
    if (root) {
        inorder(root->left);
        printf("%d ", root->data);
        inorder(root->right);
    }
}

// Размер дерева
int size(Node* root) {
    if (!root) return 0;
    return 1 + size(root->left) + size(root->right);
}

// Контрольная сумма
int checksum(Node* root) {
    if (!root) return 0;
    return root->data + checksum(root->left) + checksum(root->right);
}

// Высота дерева
int height(Node* root) {
    if (!root) return 0;
    int lh = height(root->left);
    int rh = height(root->right);
    return 1 + (lh > rh ? lh : rh);
}

// Вспомогательная для средней высоты
void depthSum(Node* root, int depth, int* sum, int* count) {
    if (!root) return;
    *sum += depth;
    (*count)++;
    depthSum(root->left, depth + 1, sum, count);
    depthSum(root->right, depth + 1, sum, count);
}

// Средняя высота
double avgDepth(Node* root) {
    int sum = 0, count = 0;
    depthSum(root, 1, &sum, &count);
    if (count == 0) return 0;
    return (double)sum / count;
}

// --- ФУНКЦИИ ПОСТРОЕНИЯ ---

// Построение ИСДП (Идеально сбалансированного дерева поиска)
Node* BuildISDP(int L, int R, int A[]) {
    if (L > R) {
        return NULL;
    } else {
        int m = (L + R) / 2;
        Node* p = createNode(A[m]);
        p->left = BuildISDP(L, m - 1, A);
        p->right = BuildISDP(m + 1, R, A);
        return p;
    }
}


// Заполнение массива
void FillInc(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        A[i] = i + 1;
    }
}

// Графический вывод (повернуто влево)
void PrintTree(Node* p, int level) {
    if (!p) return;
    PrintTree(p->right, level + 1);
    printf("%*s%d\n", level * 4, "", p->data);
    PrintTree(p->left, level + 1);
}

int main() {
    int A[N];
    FillInc(N, A); 

    // 1. ИСДП
    printf("=== ИСДП (Идеально сбалансированное дерево) ===\n");
    Node* isdpRoot = BuildISDP(0, N - 1, A);
    
    printf("Обход слева направо: ");
    inorder(isdpRoot);
    printf("\n");
    
    printf("Размер: %d\n", size(isdpRoot));
    printf("Сумма: %d\n", checksum(isdpRoot));
    printf("Высота: %d\n", height(isdpRoot));
    printf("Средняя высота: %.2f\n", avgDepth(isdpRoot));
    
    printf("\nГрафический вывод ИСДП:\n");
    PrintTree(isdpRoot, 0); // Раскомментировал вывод

    printf("\n---------------------------------------\n");


    return 0;
}