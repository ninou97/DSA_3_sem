#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
    int balance; 
} Node;

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

Node* createNode(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->balance = 0; // Новая вершина всегда сбалансирована
    return node;
}


Node* LL_Rotate(Node* p) {
    Node* q = p->left;
    
    p->balance = 0;
    q->balance = 0;
    
    p->left = q->right;
    q->right = p;
    
    return q; // q становится новым корнем поддерева
}

Node* RR_Rotate(Node* p) {
    Node* q = p->right;
    
    p->balance = 0;
    q->balance = 0;
    
    p->right = q->left;
    q->left = p;
    
    return q;
}

Node* LR_Rotate(Node* p) {
    Node* q = p->left;
    Node* r = q->right;
    
    // Пересчет балансов в зависимости от баланса r
    if (r->balance < 0) {
        p->balance = 1;
    } else {
        p->balance = 0;
    }
    
    if (r->balance > 0) {
        q->balance = -1;
    } else {
        q->balance = 0;
    }
    
    r->balance = 0;
    
    // Переброс указателей
    q->right = r->left;
    p->left = r->right;
    r->left = q;
    r->right = p;
    
    return r;
}

Node* RL_Rotate(Node* p) {
    Node* q = p->right;
    Node* r = q->left;
    
    if (r->balance > 0) {
        p->balance = -1;
    } else {
        p->balance = 0;
    }
    
    if (r->balance < 0) {
        q->balance = 1;
    } else {
        q->balance = 0;
    }
    
    r->balance = 0;
    
    p->right = r->left;
    q->left = r->right;
    r->right = q;
    r->left = p;
    
    return r;
}

// Используем указатель на grow (Рост), чтобы передавать изменение высоты вверх по рекурсии
Node* insertAVL_Rec(Node* p, int data, int* grow) {
    if (p == NULL) {
        // new(p), p->Data := D ... p->Balance := 0, Рост := ИСТИНА
        p = createNode(data);
        *grow = 1; // ИСТИНА
        return p;
    }

    if (data < p->data) {
        // Добавление в левое поддерево
        p->left = insertAVL_Rec(p->left, data, grow);
        
        if (*grow == 1) { // Если выросла левая ветвь
            if (p->balance > 0) {
                p->balance = 0;
                *grow = 0; // ЛОЖЬ
            }
            else if (p->balance == 0) {
                p->balance = -1;
                *grow = 1; // Рост продолжается
            }
            else { // p->balance == -1 (стало -2, нарушение)
                // Проверка для выбора поворота (LL или LR)
                if (p->left->balance < 0) {
                    p = LL_Rotate(p); // <LL - поворот>
                } else {
                    p = LR_Rotate(p); // <LR - поворот>
                }
                *grow = 0; // Рост := ЛОЖЬ
            }
        }
    }
    else if (data > p->data) {
        // Добавление в правое поддерево (аналогичные действия)
        p->right = insertAVL_Rec(p->right, data, grow);
        
        if (*grow == 1) { // Если выросла правая ветвь
            if (p->balance < 0) {
                p->balance = 0;
                *grow = 0;
            }
            else if (p->balance == 0) {
                p->balance = 1;
                *grow = 1;
            }
            else { // p->balance == 1 (стало +2)
                if (p->right->balance > 0) {
                    p = RR_Rotate(p); // Используем симметричный LL (RR)
                } else {
                    p = RL_Rotate(p); // Используем симметричный LR (RL)
                }
                *grow = 0;
            }
        }
    }
    // Если data == p->data, ничего не делаем (дубликаты игнорируем или обрабатываем отдельно)
    
    return p;
}

Node* insertAVL(Node* root, int data) {
    int grow = 0;
    return insertAVL_Rec(root, data, &grow);
}

// --- ФУНКЦИИ СТАТИСТИКИ ---

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

// --- ИСДП (Для сравнения) ---
Node* BuildISDP(int* arr, int start, int end) {
    if (start > end) return NULL;
    int mid = (start + end) / 2;
    Node* root = createNode(arr[mid]); 
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
    printf("\nГрафический вывод тестового АВЛ-дерева:\n");

    return 0;
}