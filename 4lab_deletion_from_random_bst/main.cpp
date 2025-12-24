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

// Графический вывод (для наглядности удаления)
void PrintTree(Node* root, int level) {
    if (root) {
        PrintTree(root->right, level + 1);
        printf("%*s%d\n", level * 4, "", root->data);
        PrintTree(root->left, level + 1);
    }
}

// --- ФУНКЦИИ ВСТАВКИ И УДАЛЕНИЯ ---

// Вставка с двойной косвенностью
void insert(Node** root, int data) {
    Node** current = root;
    while (*current != NULL) {
        if (data < (*current)->data) {
            current = &((*current)->left);
        } else if (data > (*current)->data) {
            current = &((*current)->right);
        } else {
            return; // Дубликаты не вставляем
        }
    }
    *current = createNode(data);
}

// Поиск минимального узла (нужен для удаления)
Node* findMin(Node* root) {
    while (root != NULL && root->left != NULL) {
        root = root->left;
    }
    return root;
}

Node* deleteNode(Node* root, int X) {
    // p := @Root
    // p хранит адрес указателя, который смотрит на текущий узел.
    // Сначала это адрес переменной root, потом адреса полей ->left или ->right.
    Node** p = &root;

    // DO (*p ≠ NIL)
    // Ищем узел итеративно
    while (*p != NULL) {
        if ((*p)->data < X) {
            p = &((*p)->right); // p := @((*p)→Right)
        } else if ((*p)->data > X) {
            p = &((*p)->left);  // p := @((*p)→Left)
        } else {
            break; // Нашли узел, выходим (ELSE OD)
        }
    }

    // IF (*p ≠ NIL)
    if (*p != NULL) {
        Node* q = *p; // q := *p (Узел, который удаляем)

        // 1. Если нет левого ребенка
        if (q->left == NULL) {
            *p = q->right; // *p := q→Right
        }
        // 2. Если нет правого ребенка
        else if (q->right == NULL) {
            *p = q->left;  // *p := q→Left
        }
        // 3. Есть оба ребенка
        else {
            Node* r = q->left; // r := q→Left
            Node* s = q;       // s := q

            
            if (r->right == NULL) {
                // Если r сразу максимальный (у него нет правого потомка)
                r->right = q->right; // 3)
                *p = r;              // 4)
            } else {
                // DO (r→Right ≠ NIL)
                // Идем в самую правую (большую) вершину левого поддерева
                while (r->right != NULL) {
                    s = r;        // s := r
                    r = r->right; // r := r→Right
                }

                s->right = r->left;   // 1) Родитель r (s) забирает левый хвост r
                r->left = q->left;    // 2) r забирает левых детей q
                r->right = q->right;  // 3) r забирает правых детей q
                *p = r;               // 4) Родитель q теперь указывает на r
            }
        }

        free(q); // dispose(q)
    }

    return root;
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
    int n = 100;
    int arr[100];

    // 1. Создаем массив 1..100
    for (int i = 0; i < n; i++) arr[i] = i + 1;
    
    // 2. Перемешиваем и строим СДП
    shuffle(arr, n);
    
    Node* root = NULL;
    for (int i = 0; i < n; i++) {
        insert(&root, arr[i]);
    }

    // Вывод начального состояния
    printf("=== СДП построено (100 вершин) ===\n");
    printf("Размер: %d, Высота: %d, Сумма: %d\n", size(root), height(root), checksum(root));
    printf("Обход слева направо: ");
    inorder(root);
    printf("\n\n");
    PrintTree(root, 0); 

    // 3. Цикл удаления 10 вершин
    for (int i = 0; i < 10; i++) {
        int key;
        printf("----------------------------------------\n");
        printf("Шаг %d/10. Введите число для удаления: ", i + 1);
        
        // Защита от ввода букв
        if (scanf("%d", &key) != 1) {
            break; 
        }

        deleteNode(root, key);

        // Вывод после удаления
        printf("-> Узел %d удален.\n", key);
        printf("Характеристики: Размер=%d, Высота=%d, Сумма=%d\n", 
               size(root), height(root), checksum(root));
        
        // Для проверки выводим обход (должен остаться отсортированным, но без удаленного числа)
        printf("Обход: ");
        inorder(root);
        printf("\n");
        
        // Если хочешь видеть структуру дерева, раскомментируй строку ниже:
        PrintTree(root, 0); 
    }

    return 0;
}