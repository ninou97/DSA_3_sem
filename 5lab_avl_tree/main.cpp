#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
    int height; // Храним высоту узла для балансировки
} Node;

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (ДЛЯ АВЛ ЛОГИКИ) ---

int getHeight(Node* n) {
    if (n == NULL) return 0;
    return n->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

Node* createNode(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // Новый узел всегда красный лист высоты 1
    return node;
}

// --- ФУНКЦИИ СТАТИСТИКИ (Твои стандартные) ---

// Обход слева направо
void inorder(Node* root) {
    if (root) {
        inorder(root->left);
        printf("%d ", root->data);
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

// Функция вычисления высоты "по факту" (для отчета)
int calcHeight_Recursive(Node* root) {
    if (!root) return 0;
    int lh = calcHeight_Recursive(root->left);
    int rh = calcHeight_Recursive(root->right);
    return 1 + (lh > rh ? lh : rh);
}

void depthSum(Node* root, int depth, int* sum, int* count) {
    if (!root) return;
    *sum += depth;
    (*count)++;
    depthSum(root->left, depth + 1, sum, count);
    depthSum(root->right, depth + 1, sum, count);
}

double avgDepth(Node* root) {
    int sum = 0, count = 0;
    depthSum(root, 1, &sum, &count);
    if (count == 0) return 0;
    return (double)sum / count;
}

// --- АВЛ БАЛАНСИРОВКА ---

// Правый поворот (вокруг y)
Node* rightRotate(Node* y) {
    Node* x = y->left; // x will be new root
    Node* T2 = x->right; // right branch will go to y

    x->right = y;
    y->left = T2;

    // refresh heights starting from lower node
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x; // Новый корень
}

// Левый поворот (вокруг x)
Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

// Получение баланса
int getBalance(Node* N) {
    if (N == NULL) return 0;
    return getHeight(N->left) - getHeight(N->right);
}

// Вставка в АВЛ-дерево
Node* insertAVL(Node* node, int data) {
    // 1. Обычная вставка BST
    if (node == NULL)
        return createNode(data);

    if (data < node->data)
        node->left = insertAVL(node->left, data);
    else if (data > node->data)
        node->right = insertAVL(node->right, data);
    else
        return node; // Дубликаты не нужны

    // 2. Обновляем высоту текущего предка, leafs are 0 height
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // 3. Проверяем баланс
    int balance = getBalance(node);

    // 4. Если дисбаланс, есть 4 случая:

    // Left Left Case
    if (balance > 1 && data < node->left->data)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && data > node->right->data)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && data > node->left->data) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && data < node->right->data) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// --- ИСДП (Для сравнения) ---
Node* BuildISDP(int* arr, int start, int end) {
    if (start > end) return NULL;
    int mid = (start + end) / 2;
    Node* root = createNode(arr[mid]); // createNode ставит height=1, но для ИСДП нам высота не важна
    root->left = BuildISDP(arr, start, mid - 1);
    root->right = BuildISDP(arr, mid + 1, end);
    return root;
}

// Графический вывод
void PrintTree(Node* root, int level) {
    if (root) {
        PrintTree(root->right, level + 1);
        printf("%*s%d\n", level * 4, "", root->data);
        PrintTree(root->left, level + 1);
    }
}

// Перемешивание
void shuffle(int* array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main() {
    srand(time(NULL));
    int n = 100;
    
    // Массив 1..100 для ИСДП (упорядоченный)
    int arrSorted[100];
    // Массив 1..100 для АВЛ (случайный)
    int arrRandom[100];

    for (int i = 0; i < n; i++) {
        arrSorted[i] = i + 1;
        arrRandom[i] = i + 1;
    }
    shuffle(arrRandom, n);

    // 1. Строим ИСДП
    Node* rootISDP = BuildISDP(arrSorted, 0, n - 1);

    // 2. Строим АВЛ (вставляем случайные числа)
    Node* rootAVL = NULL;
    for (int i = 0; i < n; i++) {
        rootAVL = insertAVL(rootAVL, arrRandom[i]);
    }

    // Вывод задания 3
    printf("Обход АВЛ-дерева слева направо (должен быть отсортирован):\n");
    inorder(rootAVL);
    printf("\n\n");

    // Вывод задания 4 (Сравнительная таблица)
    printf("| n=100 | Размер | Контр.сумма | Высота | Средн.высота |\n");
    printf("|-------|--------|-------------|--------|--------------|\n");

    printf("| ИСДП  | %6d | %11d | %6d | %12.2f |\n", 
           size(rootISDP), checksum(rootISDP), 
           calcHeight_Recursive(rootISDP), avgDepth(rootISDP));

    printf("| АВЛ   | %6d | %11d | %6d | %12.2f |\n", 
           size(rootAVL), checksum(rootAVL), 
           calcHeight_Recursive(rootAVL), avgDepth(rootAVL));

    // Вывод задания 5*
    printf("\nГрафический вывод АВЛ-дерева (фрагмент):\n");
    PrintTree(rootAVL, 0);
    printf("\nГрафический вывод АВЛ-дерева (фрагмент):\n");

    int arr1[] = {3, 13, 55, 21, 2, 10, 4};
    Node* root = NULL;

    for (int i=0; i<7; i++) {
        root = insertAVL(root, arr1[i]);
    }
    PrintTree(root, 0);

    return 0;
}