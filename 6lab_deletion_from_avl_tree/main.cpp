#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Глобальные счетчики для Задания 4*
int g_insertRotations = 0;
int g_deleteRotations = 0;

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
    int height;
} Node;

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

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
    node->height = 1;
    return node;
}

// Правый поворот
Node* rightRotate(Node* y, int isDeletion) {
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    // Считаем поворот
    if (isDeletion) g_deleteRotations++;
    else g_insertRotations++;

    return x;
}

// Левый поворот
Node* leftRotate(Node* x, int isDeletion) {
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    // Считаем поворот
    if (isDeletion) g_deleteRotations++;
    else g_insertRotations++;

    return y;
}

int getBalance(Node* N) {
    if (N == NULL) return 0;
    return getHeight(N->left) - getHeight(N->right);
}

// Поиск минимума (для удаления)
Node* findMin(Node* node) {
    Node* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// --- ВСТАВКА (Задание 2 из прошлого урока, нужно для построения) ---
Node* insertAVL(Node* node, int data) {
    if (node == NULL) return createNode(data);

    if (data < node->data)
        node->left = insertAVL(node->left, data);
    else if (data > node->data)
        node->right = insertAVL(node->right, data);
    else
        return node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    // Left Left
    if (balance > 1 && data < node->left->data)
        return rightRotate(node, 0);

    // Right Right
    if (balance < -1 && data > node->right->data)
        return leftRotate(node, 0);

    // Left Right
    if (balance > 1 && data > node->left->data) {
        node->left = leftRotate(node->left, 0);
        return rightRotate(node, 0);
    }

    // Right Left
    if (balance < -1 && data < node->right->data) {
        node->right = rightRotate(node->right, 0);
        return leftRotate(node, 0);
    }

    return node;
}

// --- УДАЛЕНИЕ (Задание 2) ---
Node* deleteAVL(Node* root, int key) {
    // 1. СТАНДАРТНОЕ УДАЛЕНИЕ BST
    if (root == NULL) return root;

    if (key < root->data)
        root->left = deleteAVL(root->left, key);
    else if (key > root->data)
        root->right = deleteAVL(root->right, key);
    else {
        // Узел найден
        if ((root->left == NULL) || (root->right == NULL)) {
            Node* temp = root->left ? root->left : root->right;
            if (temp == NULL) { // Нет детей
                temp = root;
                root = NULL;
            } else { // Один ребенок
                *root = *temp; // Копируем содержимое ребенка
            }
            free(temp);
        } else {
            // Два ребенка
            Node* temp = findMin(root->right);
            root->data = temp->data;
            root->right = deleteAVL(root->right, temp->data);
        }
    }

    if (root == NULL) return root;

    // 2. ОБНОВЛЕНИЕ ВЫСОТЫ
    root->height = 1 + max(getHeight(root->left), getHeight(root->right));

    // 3. ПРОВЕРКА БАЛАНСА И ПОВОРОТЫ
    int balance = getBalance(root);

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root, 1);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left, 1);
        return rightRotate(root, 1);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root, 1);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right, 1);
        return leftRotate(root, 1);
    }

    return root;
}

// --- ГРАФИКА И ОБХОДЫ ---

void inorder(Node* root) {
    if (root != NULL) {
        inorder(root->left);
        printf("%d ", root->data);
        inorder(root->right);
    }
}

void PrintTree(Node* root, int level) {
    if (root) {
        PrintTree(root->right, level + 1);
        printf("%*s%d\n", level * 4, "", root->data);
        PrintTree(root->left, level + 1);
    }
}

// Перемешивание массива
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
    int N = 100;
    int arr[100];
    for (int i = 0; i < N; i++) arr[i] = i + 1;
    
    // В задании сказано "заданных случайно", поэтому перемешиваем
    shuffle(arr, N);

    Node* root = NULL;
    
    // --- ПОСТРОЕНИЕ ---
    for (int i = 0; i < N; i++) {
        root = insertAVL(root, arr[i]);
    }
    
    printf("АВЛ-дерево построено (100 вершин).\n");
    printf("Обход слева направо: ");
    inorder(root);
    printf("\n\n");

    // Статистика для Задания 4* (Вставка)
    printf("=== Статистика по Заданию 4* ===\n");
    printf("Вставка 100 вершин:\n");
    printf("Всего поворотов: %d\n", g_insertRotations);
    printf("Отношение (Поворотов / Вставок): %.2f (Теория говорит ~0.5)\n\n", 
           (float)g_insertRotations / N);

    PrintTree(root, 0); // Можно раскомментировать для графики

    // --- УДАЛЕНИЕ 10 ВЕРШИН (Задание 3) ---
    printf("=== Удаление 10 вершин ===\n");
    int deletedCount = 0;
    
    for (int i = 0; i < 10; i++) {
        int key;
        printf("----------------------------------\n");
        printf("Шаг %d/10. Введите число для удаления: ", i + 1);
        if (scanf("%d", &key) != 1) break;
        
        root = deleteAVL(root, key);
        deletedCount++;
        
        printf("-> Дерево после удаления %d:\n", key);
        PrintTree(root, 0); // Можно раскомментировать для графики
        printf("Обход: ");
        inorder(root);
        printf("\n");
    }

    // Статистика для Задания 4* (Удаление)
    printf("\n=== Итог по удалению ===\n");
    printf("Всего удалений: %d\n", deletedCount);
    printf("Всего поворотов при удалении: %d\n", g_deleteRotations);
    if (deletedCount > 0) {
        printf("Отношение (Поворотов / Удалений): %.2f (Теория говорит ~0.2)\n", 
               (float)g_deleteRotations / deletedCount);
    }
    
    return 0;
}