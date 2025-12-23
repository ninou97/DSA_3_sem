#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define N 100

// Структура узла (добавили поле weight)
typedef struct Node {
    int data;       // Ключ (1..100)
    int weight;     // Вес (частота обращения)
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

// Стандартные функции статистики
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

// Контрольная сумма (сумма ключей)
int checksum(Node* root) {
    if (!root) return 0;
    return root->data + checksum(root->left) + checksum(root->right);
}

// Обычная высота дерева
int height(Node* root) {
    if (!root) return 0;
    int lh = height(root->left);
    int rh = height(root->right);
    return 1 + (lh > rh ? lh : rh);
}

// Взвешенная сумма путей (для проверки)
// Считаем: Глубина * Вес для каждого узла
// Корень на глубине 1
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
        // Выводим: Ключ(Вес)
        printf("%*s%d(%d)\n", level * 5, "", root->data, root->weight);
        printTree(root->left, level + 1);
    }
}

// --- АЛГОРИТМ ДОП (Optimal BST) ---

// Глобальные матрицы 
int AW[N+1][N+1]; // Weight of subtrees, from i to j
int AP[N+1][N+1]; // Взвешенная стоимость, what is the "cheapest" tree from i to j
int R[N+1][N+1];  // Корни поддеревьев, what index of AP is the one that gives us cheapest tree from i to j

// Find cheapest tree (one where weightist nodes are at the top)
// Функция заполнения матриц (Динамическое программирование)
void calculateOptimalBSTMatrix(int weights[], int n) {
    // Инициализация
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            AW[i][j] = 0;
            AP[i][j] = 0;
            R[i][j] = 0;
        }
    }

    // Заполнение диагоналей (деревья из 1 элемента)
    for (int i = 0; i < n; i++) {
        AW[i][i] = weights[i];
        AP[i][i] = weights[i]; // Стоимость одного узла = его вес * 1 (глубина)
        R[i][i] = i;
    }

    // Основной цикл по длине подцепочки (от 2 до n)
    for (int L = 2; L <= n; L++) { // L - length of range
        for (int i = 0; i <= n - L; i++) {
            int j = i + L - 1; // Конечный индекс подцепочки
            
            // AW[i][j] = Сумма весов от i до j
            AW[i][j] = AW[i][j-1] + weights[j]; // tail already computed, add new element's weight

            // Ищем корень k, который минимизирует стоимость
            int minCost = INT_MAX; // first take it as infinite
            int bestRoot = -1; // no best root yet

            // Перебираем возможные корни k от i до j
            for (int k = i; k <= j; k++) {
                // Стоимость = (стоимость левого) + (стоимость правого)
                // Если поддерево пустое, стоимость 0
                int c = ((k > i) ? AP[i][k-1] : 0) + 
                        ((k < j) ? AP[k+1][j] : 0);
                
                if (c < minCost) {
                    minCost = c;
                    bestRoot = k;
                }
            }

            // Итоговая стоимость = мин.стоимость поддеревьев + сумма весов всех узлов (т.к. все опустились на уровень вниз)
            AP[i][j] = minCost + AW[i][j];
            R[i][j] = bestRoot;
        }
    }
}

// Рекурсивное построение дерева по матрице R
Node* buildTreeFromR(int keys[], int weights[], int i, int j) {
    if (i > j) return NULL;

    int rootIndex = R[i][j];
    Node* root = createNode(keys[rootIndex], weights[rootIndex]);

    root->left = buildTreeFromR(keys, weights, i, rootIndex - 1);
    root->right = buildTreeFromR(keys, weights, rootIndex + 1, j);

    return root;
}

// Вывод части матрицы (левый верхний угол)
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

    // 1. Инициализация (упорядоченные ключи, случайные веса)
    for (int i = 0; i < N; i++) {
        keys[i] = i + 1;           // Ключи 1..100
        weights[i] = rand() % 100 + 1; // Веса 1..100
        totalWeight += weights[i];
    }

    // 2. Расчет матриц (Точный алгоритм)
    calculateOptimalBSTMatrix(weights, N);

    // 3. Построение дерева
    Node* root = buildTreeFromR(keys, weights, 0, N - 1);

    // Вывод матриц (Задание 2)
    printf("МАТРИЦЫ (фрагменты):\n");
    printMatrix(AW, 10, "AW (Сумма весов)");
    printMatrix(AP, 10, "AP (Взвешенная стоимость)");
    printMatrix(R, 10,  "R (Индексы корней)");

    // Обход (Задание 3)
    printf("\nОбход ДОП слева направо (первые 20 узлов):\n");
    // Выведем только часть, чтобы не засорять консоль
    // inorder(root); <-- так можно вывести всё
    // Ниже "костыль" чисто для красивого вывода части, как в примере:
    int cnt = 0; print_partial(root, &cnt);
    printf("...\n");

    // Расчет характеристик (Задание 4)
    int tree_size = size(root);
    int tree_sum = checksum(root); // Сумма ключей
    int tree_height = height(root);
    
    // Считаем средневзвешенную высоту по факту построения
    int wPathSum = 0;
    weightedPathSum(root, 1, &wPathSum);
    double realAvgHeight = (double)wPathSum / totalWeight;

    // Считаем теоретическую высоту из матрицы
    // AP[0][N-1] хранит минимальную взвешенную длину пути
    // AW[0][N-1] хранит сумму всех весов
    double theoreticalAvgHeight = (double)AP[0][N-1] / AW[0][N-1];

    printf("\nТАБЛИЦА ХАРАКТЕРИСТИК:\n");
    printf("| n=100 | Размер | Контр.Сумма | Высота | Средн.взвеш.высота |\n");
    printf("|-------|--------|-------------|--------|--------------------|\n");
    printf("| ДОП   | %6d | %11d | %6d | %18.4f |\n", 
           tree_size, tree_sum, tree_height, realAvgHeight);

    // Проверка (Задание 3)
    printf("\nПРОВЕРКА АЛГОРИТМА:\n");
    printf("Из матрицы (AP/AW): %.4f\n", theoreticalAvgHeight);
    printf("По факту (TreeScan): %.4f\n", realAvgHeight);
    
    if (abs(realAvgHeight - theoreticalAvgHeight) < 0.0001) 
        printf("RESULT: Совпадает! Алгоритм верен.\n");
    else 
        printf("RESULT: Ошибка!\n");

    // Графика (Задание 5*)
    printf("\nГрафическое изображение (фрагмент):\n");
    printTree(root, 0);

    return 0;
}