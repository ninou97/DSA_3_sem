#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>

#define N 100

typedef struct Node {
    int data;       // Было key
    int weight;
    struct Node* left;
    struct Node* right;
} Node;

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

Node* createNode(int data, int weight) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->weight = weight;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Обход слева направо
void inorder(Node* root) {
    if (root) {
        inorder(root->left);
        printf("%d ", root->data); // Выводим только ключ для краткости
        inorder(root->right);
    }
}

// Статистика (как в прошлых задачах)
int size(Node* root) {
    if (!root) return 0;
    return 1 + size(root->left) + size(root->right);
}

int checksum(Node* root) {
    if (!root) return 0;
    return root->data + checksum(root->left) + checksum(root->right);
}

int height(Node* root) {
    if (!root) return 0;
    int lh = height(root->left);
    int rh = height(root->right);
    return 1 + (lh > rh ? lh : rh);
}

// Сумма взвешенных длин путей (Depth * Weight)
void weightedPathSum(Node* root, int depth, int* sum) {
    if (!root) return;
    *sum += depth * root->weight;
    weightedPathSum(root->left, depth + 1, sum);
    weightedPathSum(root->right, depth + 1, sum);
}

// Графический вывод (Задание 5*)
void printTree(Node* root, int level) {
    if (root) {
        printTree(root->right, level + 1);
        printf("%*s%d(%d)\n", level * 5, "", root->data, root->weight);
        printTree(root->left, level + 1);
    }
}

void freeTree(Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// --- 1. ТОЧНЫЙ АЛГОРИТМ (ДОП / OBST) ---
// Нужен для сравнения в таблице

int AW[N+1][N+1];
int AP[N+1][N+1];
int R[N+1][N+1];

// Вспомогательная для восстановления дерева из матрицы R
Node* buildTreeFromR(int keys[], int weights[], int i, int j) {
    if (i > j) return NULL;
    int rootIdx = R[i][j];
    Node* root = createNode(keys[rootIdx], weights[rootIdx]);
    root->left = buildTreeFromR(keys, weights, i, rootIdx - 1);
    root->right = buildTreeFromR(keys, weights, rootIdx + 1, j);
    return root;
}

Node* buildOptimalBST(int keys[], int weights[], int n) {
    // Инициализация
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            AW[i][j] = AP[i][j] = R[i][j] = 0;
        }
    }
    for (int i = 0; i < n; i++) {
        AW[i][i] = AP[i][i] = weights[i];
        R[i][i] = i;
    }

    // Расчет матриц
    for (int L = 2; L <= n; L++) {
        for (int i = 0; i <= n - L; i++) {
            int j = i + L - 1;
            AW[i][j] = AW[i][j-1] + weights[j];
            
            int minCost = INT_MAX;
            int bestRoot = -1;
            
            // Оптимизация Кнута: ищем k только между R[i][j-1] и R[i+1][j]
            // Но для простоты оставим полный перебор, для N=100 это быстро
            for (int k = i; k <= j; k++) {
                int c = ((k > i) ? AP[i][k-1] : 0) + 
                        ((k < j) ? AP[k+1][j] : 0);
                if (c < minCost) {
                    minCost = c;
                    bestRoot = k;
                }
            }
            AP[i][j] = minCost + AW[i][j];
            R[i][j] = bestRoot;
        }
    }
    return buildTreeFromR(keys, weights, 0, n - 1);
}

// --- 2. ПРИБЛИЖЕННЫЙ АЛГОРИТМ А1 (Max Weight) ---
// Корнем становится элемент с максимальным весом
Node* buildA1(int keys[], int weights[], int start, int end) {
    if (start > end) return NULL;

    // Ищем индекс с максимальным весом в диапазоне
    int maxIdx = start;
    for (int i = start + 1; i <= end; i++) {
        if (weights[i] > weights[maxIdx]) {
            maxIdx = i;
        }
    }

    Node* root = createNode(keys[maxIdx], weights[maxIdx]);
    root->left = buildA1(keys, weights, start, maxIdx - 1);
    root->right = buildA1(keys, weights, maxIdx + 1, end);
    
    return root;
}

// --- 3. ПРИБЛИЖЕННЫЙ АЛГОРИТМ А2 (Balance Weight) ---
// Корнем становится элемент, делящий вес диапазона примерно пополам
Node* buildA2(int keys[], int weights[], int start, int end) {
    if (start > end) return NULL;

    // Считаем общий вес диапазона
    int totalWeight = 0;
    for (int i = start; i <= end; i++) {
        totalWeight += weights[i];
    }

    // Ищем элемент, на котором накапливаемая сумма достигнет половины
    int currentSum = 0;
    int rootIdx = start;
    
    // идем пока сумма не станет >= половины
    for (int i = start; i <= end; i++) {
        currentSum += weights[i];
        if (currentSum >= totalWeight / 2) { // Порог половины
            rootIdx = i;
            break; 
        }
    }

    Node* root = createNode(keys[rootIdx], weights[rootIdx]);
    root->left = buildA2(keys, weights, start, rootIdx - 1);
    root->right = buildA2(keys, weights, rootIdx + 1, end);

    return root;
}

// --- MAIN ---

int main() {
    srand(time(NULL));
    int keys[N];
    int weights[N];
    int totalWeight = 0;

    // Генерация данных (1..100)
    for (int i = 0; i < N; i++) {
        keys[i] = i + 1;
        weights[i] = rand() % 100 + 1;
        totalWeight += weights[i];
    }

    printf("Построение деревьев для N=%d...\n\n", N);

    // 1. Строим ДОП (Эталон)
    Node* rootOPT = buildOptimalBST(keys, weights, N);

    // 2. Строим А1
    Node* rootA1 = buildA1(keys, weights, 0, N - 1);

    // 3. Строим А2
    Node* rootA2 = buildA2(keys, weights, 0, N - 1);

    // Вывод обходов (Задание 3)
    printf("Обход ДОП (слева направо): ");
    inorder(rootOPT); printf("\n\n");

    printf("Обход А1 (слева направо):  ");
    inorder(rootA1); printf("\n\n");

    printf("Обход А2 (слева направо):  ");
    inorder(rootA2); printf("\n\n");

    // Расчет характеристик
    int sizeOPT = size(rootOPT);
    int sumOPT = checksum(rootOPT);
    int hOPT = height(rootOPT);
    int wSumOPT = 0; weightedPathSum(rootOPT, 1, &wSumOPT);
    double avgH_OPT = (double)wSumOPT / totalWeight;

    int sizeA1 = size(rootA1);
    int sumA1 = checksum(rootA1);
    int hA1 = height(rootA1);
    int wSumA1 = 0; weightedPathSum(rootA1, 1, &wSumA1);
    double avgH_A1 = (double)wSumA1 / totalWeight;

    int sizeA2 = size(rootA2);
    int sumA2 = checksum(rootA2);
    int hA2 = height(rootA2);
    int wSumA2 = 0; weightedPathSum(rootA2, 1, &wSumA2);
    double avgH_A2 = (double)wSumA2 / totalWeight;

    // Вывод Таблицы (Задание 4)
    printf("СРАВНИТЕЛЬНАЯ ТАБЛИЦА:\n");
    printf("| Alg   | Размер | Контр.Сумма | Высота | Средн.взвеш.высота | Отклонение |\n");
    printf("|-------|--------|-------------|--------|--------------------|------------|\n");
    printf("| ДОП   | %6d | %11d | %6d | %18.4f |      0.00%% |\n", 
           sizeOPT, sumOPT, hOPT, avgH_OPT);
    printf("| A1    | %6d | %11d | %6d | %18.4f | %9.2f%% |\n", 
           sizeA1, sumA1, hA1, avgH_A1, (avgH_A1/avgH_OPT - 1)*100);
    printf("| A2    | %6d | %11d | %6d | %18.4f | %9.2f%% |\n", 
           sizeA2, sumA2, hA2, avgH_A2, (avgH_A2/avgH_OPT - 1)*100);

    // Задание 5*: Графика
    printf("\nГрафический вывод ДОП (фрагмент):\n");
    printTree(rootOPT, 0);
    printf("-------------------------------------------------------------------\n");
    printf("\nГрафический вывод А1 (фрагмент):\n");
    printTree(rootA1, 0);
    printf("-------------------------------------------------------------------\n");
    printf("\nГрафический вывод А2 (фрагмент):\n");
    printTree(rootA2, 0);

    // Чистим память
    freeTree(rootOPT);
    freeTree(rootA1);
    freeTree(rootA2);

    return 0;
}