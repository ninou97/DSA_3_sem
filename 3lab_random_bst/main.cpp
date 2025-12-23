#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

// Функция создания узла
Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (Твои стандартные) ---

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

int height(Node* root) {
    if (!root) return 0;
    int lh = height(root->left);
    int rh = height(root->right);
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

// Графический вывод (Задание 5*)
void PrintTree(Node* root, int level) {
    if (root) {
        PrintTree(root->right, level + 1);
        printf("%*s%d\n", level * 4, "", root->data);
        PrintTree(root->left, level + 1);
    }
}

// --- ФУНКЦИИ ДЛЯ ЭТОЙ ЗАДАЧИ ---

// 1. Построение ИСДП (для сравнения из таблицы)
Node* BuildISDP(int* arr, int start, int end) {
    if (start > end) return NULL;
    int mid = (start + end) / 2;
    Node* root = createNode(arr[mid]);
    root->left = BuildISDP(arr, start, mid - 1);
    root->right = BuildISDP(arr, mid + 1, end);
    return root;
}

// 2. Вставка в СДП: Рекурсивно
Node* insertRecursive(Node* root, int data) {
    if (root == NULL) {
        return createNode(data);
    }
    if (data < root->data) {
        root->left = insertRecursive(root->left, data);
    } else if (data > root->data) {
        root->right = insertRecursive(root->right, data);
    }
    return root;
}

// 2. Вставка в СДП: Двойная косвенность (Double Indirect)
// Используем указатель на указатель (Node**), чтобы менять саму ссылку
void insertDoubleIndirect(Node** root, int data) {
    Node** current = root;
    while (*current != NULL) {
        if (data < (*current)->data) {
            current = &((*current)->left); // current now points to left child pointer of current's pointee
        } else if (data > (*current)->data) {
            current = &((*current)->right);
        } else {
            return; // Дубликаты игнорируем
        }
    }
    *current = createNode(data); // Create a parent or a child node, function hits this no matter what
}

// Перемешивание массива (для случайности)
void shuffle(int* array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main() {
    srand(time(NULL)); // Инициализация генератора случайных чисел
    int n = 100;
    int arr[100];

    // Заполняем массив 1..100 по порядку
    for (int i = 0; i < n; i++) arr[i] = i + 1;

    // --- 1. Строим ИСДП (эталон) ---
    // Строим из ОТСОРТИРОВАННОГО массива
    Node* rootISDP = BuildISDP(arr, 0, n - 1);

    // --- 2. Готовим данные для СДП ---
    shuffle(arr, n); // Перемешиваем массив случайно

    // --- 3. Строим СДП1 (Рекурсивно) ---
    Node* rootSDP1 = NULL;
    for (int i = 0; i < n; i++) {
        rootSDP1 = insertRecursive(rootSDP1, arr[i]);
    }

    // --- 4. Строим СДП2 (Двойная косвенность) ---
    Node* rootSDP2 = NULL;
    for (int i = 0; i < n; i++) {
        insertDoubleIndirect(&rootSDP2, arr[i]);
    }

    // Вывод обходов (проверка, что это деревья поиска - должны быть отсортированы)
    printf("Обход ИСДП: "); inorder(rootISDP); printf("\n\n");
    printf("Обход СДП1: "); inorder(rootSDP1); printf("\n\n");
    // СДП2 выводить нет смысла, оно будет идентично СДП1, так как входные данные те же

    // Таблица сравнения
    printf("| n=100 | Размер | Контр.сумма | Высота | Средн.высота |\n");
    printf("|-------|--------|-------------|--------|--------------|\n");

    printf("| ИСДП  | %6d | %11d | %6d | %12.2f |\n", 
           size(rootISDP), checksum(rootISDP), height(rootISDP), avgDepth(rootISDP));

    printf("| СДП1  | %6d | %11d | %6d | %12.2f |\n", 
           size(rootSDP1), checksum(rootSDP1), height(rootSDP1), avgDepth(rootSDP1));

    printf("| СДП2  | %6d | %11d | %6d | %12.2f |\n", 
           size(rootSDP2), checksum(rootSDP2), height(rootSDP2), avgDepth(rootSDP2));


    // Графический вывод (пример для СДП)
    printf("\nГрафический вывод СДП1 (фрагмент):\n");
    PrintTree(rootSDP1, 0);

    return 0;
}