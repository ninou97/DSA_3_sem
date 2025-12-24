#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Vertical and horizontal rebalance
int VR = 1; // поддерево выросло по высоте
int HR = 1; // поддерево выросло в ширину (наклон)

// === СТРУКТУРЫ ===

// Узел АВЛ-дерева (хранит высоту)
typedef struct AVLNode {
    int data;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

// Узел ДБД-дерева (хранит баланс/связь)
typedef struct DBDNode {
    int data;
    int bal; // 0 - vertical, 1 - правая горизонтальная ссылка
    struct DBDNode *left;
    struct DBDNode *right;
} DBDNode;

// === ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ===

int max(int a, int b) {
    return (a > b) ? a : b;
}

// === ЧАСТЬ 1: АВЛ-ДЕРЕВО (Для сравнения) ===

int getHeightAVL(AVLNode* node) {
    return node ? node->height : 0;
}

int getBalanceAVL(AVLNode* node) {
    return node ? getHeightAVL(node->left) - getHeightAVL(node->right) : 0;
}

AVLNode* createAVLNode(int data) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->data = data;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

// Повороты для АВЛ
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(getHeightAVL(y->left), getHeightAVL(y->right)) + 1;
    x->height = max(getHeightAVL(x->left), getHeightAVL(x->right)) + 1;
    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(getHeightAVL(x->left), getHeightAVL(x->right)) + 1;
    y->height = max(getHeightAVL(y->left), getHeightAVL(y->right)) + 1;
    return y;
}

AVLNode* insertAVL(AVLNode* node, int data) {
    if (node == NULL) return createAVLNode(data);

    if (data < node->data)
        node->left = insertAVL(node->left, data);
    else if (data > node->data)
        node->right = insertAVL(node->right, data);
    else
        return node;

    node->height = 1 + max(getHeightAVL(node->left), getHeightAVL(node->right));
    int balance = getBalanceAVL(node);

    if (balance > 1 && data < node->left->data)
        return rotateRight(node);
    if (balance < -1 && data > node->right->data)
        return rotateLeft(node);
    if (balance > 1 && data > node->left->data) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && data < node->right->data) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

// === ЧАСТЬ 2: ДБД-ДЕРЕВО (Основное задание) ===

DBDNode* createDBDNode(int data) {
    DBDNode* node = (DBDNode*)malloc(sizeof(DBDNode));
    node->data = data;
    node->left = node->right = NULL;
    node->bal = 0;
    return node;
}

// Использует глобальные переменные VR и HR для управления балансировкой
DBDNode* insertDBD(DBDNode *p, int data) {
    DBDNode *q;
    
    if (p == NULL) {
        p = createDBDNode(data);
        VR = 1; // Vertical Rebalance needed
        return p;
    }
    
    if (data < p->data) {
        p->left = insertDBD(p->left, data);
        if (VR == 1) {
            if (p->bal == 0) { // node was vertical, now needs right rotation
                q = p->left;
                p->left = q->right;
                q->right = p;
                p = q;
                q->bal = 1;
                VR = 0;
                HR = 1; 
            } else {  // if already horizontal
                p->bal = 0;
                VR = 1; // node "pushed" up
                HR = 0;
            }
        } else {
            HR = 0; // doesn't need anything
        }
    } else if (data > p->data) {
        p->right = insertDBD(p->right, data);
        if (VR == 1) { 
            p->bal = 1; // make the horizontal row instead
            HR = 1;
            VR = 0;
        } else if (HR == 1) {
            if (p->bal == 1) { // left rotation and go up
                // Ситуация: У нас уже была горизонтальная связь, и снова рост вправо.
                // Страница переполнена -> Сплит (Левый поворот).
                q = p->right;
                p->bal = 0;
                q->bal = 0;
                p->right = q->left;
                q->left = p;
                p = q;
                VR = 1; // middle element goes up
                HR = 0;
            } else {
                HR = 0;
            }
        }
    }
    return p;
}


void inorderAVL(AVLNode* root) {
    if (root) {
        inorderAVL(root->left);
        printf("%d ", root->data);
        inorderAVL(root->right);
    }
}

int sizeAVL(AVLNode* root) {
    if (!root) return 0;
    return 1 + sizeAVL(root->left) + sizeAVL(root->right);
}

int checksumAVL(AVLNode* root) {
    if (!root) return 0;
    return root->data + checksumAVL(root->left) + checksumAVL(root->right);
}

// Высота для АВЛ у нас хранится в node->height, но для честности посчитаем рекурсивно
int calcHeightAVL(AVLNode* root) {
    if (!root) return 0;
    int lh = calcHeightAVL(root->left);
    int rh = calcHeightAVL(root->right);
    return 1 + (lh > rh ? lh : rh);
}

void depthSumAVL(AVLNode* root, int depth, int* sum, int* count) {
    if (!root) return;
    *sum += depth;
    (*count)++;
    depthSumAVL(root->left, depth + 1, sum, count);
    depthSumAVL(root->right, depth + 1, sum, count);
}

double avgDepthAVL(AVLNode* root) {
    int sum = 0, count = 0;
    depthSumAVL(root, 1, &sum, &count);
    return (count == 0) ? 0 : (double)sum / count;
}

// --- Для ДБД ---
void inorderDBD(DBDNode* root) {
    if (root) {
        inorderDBD(root->left);
        printf("%d ", root->data);
        inorderDBD(root->right);
    }
}

int sizeDBD(DBDNode* root) {
    if (!root) return 0;
    return 1 + sizeDBD(root->left) + sizeDBD(root->right);
}

int checksumDBD(DBDNode* root) {
    if (!root) return 0;
    return root->data + checksumDBD(root->left) + checksumDBD(root->right);
}

// Задание 5*: Функция вычисления количества уровней (Высота)
int calcHeightDBD(DBDNode* root) {
    if (!root) return 0;
    int lh = calcHeightDBD(root->left);
    int rh = calcHeightDBD(root->right);
    return 1 + (lh > rh ? lh : rh);
}

void depthSumDBD(DBDNode* root, int depth, int* sum, int* count) {
    if (!root) return;
    *sum += depth;
    (*count)++;
    depthSumDBD(root->left, depth + 1, sum, count);
    depthSumDBD(root->right, depth + 1, sum, count);
}

double avgDepthDBD(DBDNode* root) {
    int sum = 0, count = 0;
    depthSumDBD(root, 1, &sum, &count);
    return (count == 0) ? 0 : (double)sum / count;
}

// Задание 6*: Графический вывод ДБД
void printTreeDBD(DBDNode* root, int level) {
    if (root) {
        printTreeDBD(root->right, level + 1);
        printf("%*s%d\n", level * 4, "", root->data);
        printTreeDBD(root->left, level + 1);
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
    int N = 100;
    int numbers[100];
    
    // Заполняем и перемешиваем
    for (int i = 0; i < N; i++) numbers[i] = i + 1;
    shuffle(numbers, N);
    
    AVLNode* avlRoot = NULL;
    DBDNode* dbdRoot = NULL;

    // Построение деревьев
    for (int i = 0; i < N; i++) {
        // Строим АВЛ
        avlRoot = insertAVL(avlRoot, numbers[i]);
        
        // Строим ДБД (Сбрасываем флаги перед каждой вставкой для корневого вызова)
        VR = 1; HR = 1; 
        dbdRoot = insertDBD(dbdRoot, numbers[i]);
    }
    
    // Вывод заданий 2 и 3
    printf("Обход АВЛ (слева направо): ");
    inorderAVL(avlRoot);
    printf("\n\n");
    
    printf("Обход ДБД (слева направо): ");
    inorderDBD(dbdRoot);
    printf("\n\n");
    
    // Вывод задания 4 (Таблица сравнения)
    printf("n=100    Размер  Контр.Сумма  Высота  Средн.высота\n");
    printf("--------------------------------------------------\n");
    printf("АВЛ      %-7d %-12d %-7d %-12.2f\n", 
           sizeAVL(avlRoot), checksumAVL(avlRoot), 
           calcHeightAVL(avlRoot), avgDepthAVL(avlRoot));
           
    printf("ДБД      %-7d %-12d %-7d %-12.2f\n", 
           sizeDBD(dbdRoot), checksumDBD(dbdRoot), 
           calcHeightDBD(dbdRoot), avgDepthDBD(dbdRoot));
    
    // Вывод заданий 5* и 6*
    printf("\nЗадание 5*: Количество уровней в ДБД равно %d\n", calcHeightDBD(dbdRoot));
    
    printf("\nЗадание 6*: Графическое изображение ДБД (фрагмент):\n");
    printTreeDBD(dbdRoot, 0);
    
    return 0;
}