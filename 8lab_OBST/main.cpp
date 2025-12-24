#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define N 100

// Структура узла
typedef struct Node {
    int data;       // Ключ
    int weight;     // Вес
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

void inorder(Node* root) {
    if (root) {
        inorder(root->left);
        printf("%d(%d) ", root->data, root->weight);
        inorder(root->right);
    }
}

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

void weightedPathSum(Node* root, int depth, int* sum) {
    if (!root) return;
    *sum += depth * root->weight;
    weightedPathSum(root->left, depth + 1, sum);
    weightedPathSum(root->right, depth + 1, sum);
}

void printTree(Node* root, int level) {
    if (root) {
        printTree(root->right, level + 1);
        printf("%*s%d(%d)\n", level * 5, "", root->data, root->weight);
        printTree(root->left, level + 1);
    }
}


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

// Вывод части матрицы
void printMatrix(int matrix[][N+1], int size, const char* name) {
    printf("\n%s (первые 10x10):\n", name);
    printf("   ");
    for(int j=0; j<size; j++) printf("%4d ", j);
    printf("\n");
    for(int i=0; i<size; i++) {
        printf("%2d:", i);
        for(int j=0; j<size; j++) {
            if (j >= i) printf("%4d ", matrix[i][j]);
            else printf("     ");
        }
        printf("\n");
    }
}

void print_partial(Node* r, int* c) { if(!r || *c >= 20) return;
    print_partial(r->left, c);
    if(*c < 20) { printf("%d(%d) ", r->data, r->weight); (*c)++; }
    print_partial(r->right, c);
}

int main() {
    srand(time(NULL));
    
    int keys[N];
    int weights[N];
    int totalWeight = 0;

    for (int i = 0; i < N; i++) {
        keys[i] = i + 1;
        weights[i] = rand() % 100 + 1;
        totalWeight += weights[i];
    }

    calculateOptimalBSTMatrix(weights, N);

    // Параметры L=0, R=N соответствуют полному диапазону
    Node* root = buildTreeFromR(keys, weights, 0, N);

    // Вывод матриц
    printf("МАТРИЦЫ (фрагменты):\n");
    printMatrix(AW, 10, "AW (Сумма весов)");
    printMatrix(AP, 10, "AP (Взвешенная стоимость)");
    printMatrix(R, 10,  "R (Индексы корней)");

    // Обход
    printf("\nОбход ДОП слева направо (первые 20 узлов):\n");
    int cnt = 0; print_partial(root, &cnt);
    printf("...\n");

    // Расчет характеристик
    int tree_size = size(root);
    int tree_sum = checksum(root);
    int tree_height = height(root);
    
    int wPathSum = 0;
    weightedPathSum(root, 1, &wPathSum);
    double realAvgHeight = (double)wPathSum / totalWeight;

    // Теоретическая высота = Стоимость всего дерева / Общий вес
    double theoreticalAvgHeight = (double)AP[0][N] / AW[0][N];

    printf("\nТАБЛИЦА ХАРАКТЕРИСТИК:\n");
    printf("| n=100 | Размер | Контр.Сумма | Высота | Средн.взвеш.высота |\n");
    printf("|-------|--------|-------------|--------|--------------------|\n");
    printf("| ДОП   | %6d | %11d | %6d | %18.4f |\n", 
           tree_size, tree_sum, tree_height, realAvgHeight);

    printf("\nПРОВЕРКА АЛГОРИТМА:\n");
    printf("Из матрицы (AP/AW): %.4f\n", theoreticalAvgHeight);
    printf("По факту (TreeScan): %.4f\n", realAvgHeight);
    
    if (abs(realAvgHeight - theoreticalAvgHeight) < 0.0001) 
        printf("RESULT: Совпадает! Алгоритм верен.\n");
    else 
        printf("RESULT: Ошибка!\n");

    printf("\nГрафическое изображение (фрагмент):\n");
    printTree(root, 0);

    return 0;
}