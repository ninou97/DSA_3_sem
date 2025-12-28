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

void calculateOptimalBSTMatrix(int weights[], int n) {
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            AW[i][j] = 0;
            AP[i][j] = 0;
            R[i][j] = 0;
        }
    }

    // 2. Вычисление матрицы весов AW
    // weights в C идут с 0, поэтому weights[j-1]
    for (int i = 0; i <= n; i++) {
        for (int j = i + 1; j <= n; j++) {
            AW[i][j] = AW[i][j-1] + weights[j-1];
        }
    }

    
    // Инициализация диагоналей (h=1)
    for (int i = 0; i < n; i++) {
        int j = i + 1;
        AP[i][j] = AW[i][j];
        R[i][j] = j;
    }

    // Основной цикл по длине поддерева h
    for (int h = 2; h <= n; h++) {
        for (int i = 0; i <= n - h; i++) {
            int j = i + h;
            
            // m - начало диапазона поиска корня
            // Используем R[i][j-1] как нижнюю границу
            int m = R[i][j-1];
            
            // Предел поиска - R[i+1][j]
            int limit = R[i+1][j];

            // Здесь k выступает в роли кандидата на корень
            
            // Начальное значение минимума берем для k = m
            int minVal = AP[i][m-1] + AP[m][j];
            int bestK = m;

            // Цикл поиска k 
            for (int k = m + 1; k <= limit; k++) {
                int x = AP[i][k-1] + AP[k][j];
                if (x < minVal) {
                    minVal = x;
                    bestK = k; // запоминаем индекс
                }
            }

            // Запись результатов
            AP[i][j] = minVal + AW[i][j];
            R[i][j] = bestK;
        }
    }
}

// Создание дерева
// L, R - границы (0..N). Корень находится по индексу k = R[L][R].
Node* buildTreeFromR(int keys[], int weights[], int L, int R_idx) {
    if (L < R_idx) {
        int k = R[L][R_idx]; // Индекс корня (в 1-based, 1..N)
        
        // В keys и weights данные лежат с 0, поэтому k-1
        Node* root = createNode(keys[k-1], weights[k-1]);
        
        // Рекурсия: (L, k-1) и (k, R) 
        root->left = buildTreeFromR(keys, weights, L, k - 1);
        root->right = buildTreeFromR(keys, weights, k, R_idx);
        
        return root;
    }
    return NULL;
}

// --- 2. ПРИБЛИЖЕННЫЙ АЛГОРИТМ А1 (Max Weight) ---
// Корнем становится элемент с максимальным весом
Node* buildA1old(int keys[], int weights[], int start, int end) {
    if (start > end) return NULL;

    // Ищем индекс с максимальным весом в диапазоне
    int maxIdx = start;
    for (int i = start + 1; i <= end; i++) {
        if (weights[i] > weights[maxIdx]) {
            maxIdx = i;
        }
    }

    Node* root = createNode(keys[maxIdx], weights[maxIdx]);
    root->left = buildA1old(keys, weights, start, maxIdx - 1);
    root->right = buildA1old(keys, weights, maxIdx + 1, end);
    
    return root;
}

// Псевдокод "Добавление в СДП" подразумевает обычную вставку в бинарное дерево поиска
Node* insertSDP(Node* root, int key, int weight) {
    if (root == NULL) {
        return createNode(key, weight);
    }
    if (key < root->data) {
        root->left = insertSDP(root->left, key, weight);
    } else if (key > root->data) {
        root->right = insertSDP(root->right, key, weight);
    }
    return root;
}

// --- АЛГОРИТМ А1 (Строго по псевдокоду) ---
Node* buildA1(int keys[], int weights[], int n) {
    Node* root = NULL;
    
    // V.use – логическая переменная... (эмулируем массивом)
    int* used = (int*)calloc(n, sizeof(int)); // Инициализируем 0 (ЛОЖЬ)

    // DO (i = 1,...,n) - внешний цикл, нужно добавить n вершин
    for (int i = 0; i < n; i++) {
        
        int maxW = -1;
        int index = -1;

        // DO (j = 1,...,n) - поиск максимума среди неиспользованных
        for (int j = 0; j < n; j++) {
            // IF (V[j].w > max и V[j].use = ЛОЖЬ)
            if (!used[j] && weights[j] > maxW) {
                maxW = weights[j];
                index = j;
            }
        }

        if (index != -1) {
            // V[index].use := ИСТИНА
            used[index] = 1;
            
            // Добавление в СДП (Root, V[index])
            root = insertSDP(root, keys[index], weights[index]);
        }
    }

    free(used);
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
        // Условие: сумма ДО элемента меньше половины, 
        // а сумма С элементом уже больше или равна половине.
        if (currentSum < (double)totalWeight/2.0 && (currentSum + weights[i]) >= (double)totalWeight/2.0) {
            rootIdx = i;
            break;
        }
        currentSum += weights[i];
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

    calculateOptimalBSTMatrix(weights, N);

    // 1. Строим ДОП (Эталон)
    Node* rootOPT = buildTreeFromR(keys, weights, 0, N);

    // 2. Строим А1
    Node* rootA1 = buildA1(keys, weights, N);

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