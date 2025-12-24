#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Глобальные счетчики (Задание 4*)
int g_insertRotations = 0;
int g_deleteRotations = 0;

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
    int balance; // balance (-1, 0, 1)
} Node;


Node* createNode(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->balance = 0;
    return node;
}

int calcHeight_Recursive(Node* root) {
    if (!root) return 0;
    int lh = calcHeight_Recursive(root->left);
    int rh = calcHeight_Recursive(root->right);
    return 1 + (lh > rh ? lh : rh);
}

// rotate to left
Node* rotate_left_ptr(Node* p) {
    Node* q = p->right;
    p->right = q->left;
    q->left = p;
    return q;
}

// rotate to right
Node* rotate_right_ptr(Node* p) {
    Node* q = p->left;
    p->left = q->right;
    q->right = p;
    return q;
}

// ll imbalance, rotate to right
Node* LL_Rotate_Insert(Node* p) {
    g_insertRotations++;
    Node* q = p->left;
    p->balance = 0;
    q->balance = 0;
    return rotate_right_ptr(p);
}

// rr imbalance, rotate to left
Node* RR_Rotate_Insert(Node* p) {
    g_insertRotations++;
    Node* q = p->right;
    p->balance = 0;
    q->balance = 0;
    return rotate_left_ptr(p);
}

// lr imbalance
Node* LR_Rotate_Insert(Node* p) {
    g_insertRotations++;
    Node* q = p->left;
    Node* r = q->right;
    if (r->balance < 0) p->balance = 1; else p->balance = 0;
    if (r->balance > 0) q->balance = -1; else q->balance = 0;
    r->balance = 0;
    p->left = rotate_left_ptr(q);
    return rotate_right_ptr(p);
}

// rl imbalance
Node* RL_Rotate_Insert(Node* p) {
    g_insertRotations++;
    Node* q = p->right;
    Node* r = q->left;
    if (r->balance > 0) p->balance = -1; else p->balance = 0;
    if (r->balance < 0) q->balance = 1; else q->balance = 0;
    r->balance = 0;
    p->right = rotate_right_ptr(q);
    return rotate_left_ptr(p);
}

// --- БАЛАНСИРОВКА ПРИ УДАЛЕНИИ  ---
// decrease (Уменьшение) - флаг, уменьшилась ли высота поддерева

void BL(Node** p, int* decrease) {
    if ((*p)->balance == -1) {
        (*p)->balance = 0;
        *decrease = 1; // Уменьшение := ИСТИНА
    } else if ((*p)->balance == 0) {
        (*p)->balance = 1;
        *decrease = 0; // Уменьшение := ЛОЖЬ
    } else { // balance == 1, нарушение справа
        Node* rightChild = (*p)->right;
        if (rightChild->balance >= 0) {
            // RR-поворот
            Node* q = rightChild;
            if (q->balance == 0) {
                (*p)->balance = 1;
                q->balance = -1;
                *decrease = 0;
            } else { // Высота уменьшается
                (*p)->balance = 0;
                q->balance = 0;
                *decrease = 1;
            }
            *p = rotate_left_ptr(*p);
            g_deleteRotations++; 
        } else {
            // RL-поворот
            Node* q = (*p)->right;
            Node* r = q->left;
            if (r->balance > 0) (*p)->balance = -1; else (*p)->balance = 0;
            if (r->balance < 0) q->balance = 1; else q->balance = 0;
            r->balance = 0;
            
            (*p)->right = rotate_right_ptr(q);
            *p = rotate_left_ptr(*p);
            
            *decrease = 1; 
            g_deleteRotations++; 
        }
    }
}

void BR(Node** p, int* decrease) {
    if ((*p)->balance == 1) {
        (*p)->balance = 0;
        *decrease = 1;
    } else if ((*p)->balance == 0) {
        (*p)->balance = -1;
        *decrease = 0;
    } else { // balance == -1, нарушение слева
        Node* leftChild = (*p)->left;
        if (leftChild->balance <= 0) {
            // LL1-поворот
            Node* q = leftChild;
            if (q->balance == 0) {
                (*p)->balance = -1;
                q->balance = 1;
                *decrease = 0;
            } else {
                (*p)->balance = 0;
                q->balance = 0;
                *decrease = 1;
            }
            *p = rotate_right_ptr(*p);
            g_deleteRotations++;
        } else {
            // LR-поворот
            Node* q = (*p)->left;
            Node* r = q->right;
            if (r->balance < 0) (*p)->balance = 1; else (*p)->balance = 0;
            if (r->balance > 0) q->balance = -1; else q->balance = 0;
            r->balance = 0;

            (*p)->left = rotate_left_ptr(q);
            *p = rotate_right_ptr(*p);
            
            *decrease = 1;
            g_deleteRotations++;
        }
    }
}

// --- ВСТАВКА ---
Node* insertAVL_Rec(Node* p, int data, int* grow) {
    if (!p) {
        *grow = 1;
        return createNode(data);
    }
    if (data < p->data) {
        p->left = insertAVL_Rec(p->left, data, grow);
        if (*grow) {
            if (p->balance > 0) { p->balance = 0; *grow = 0; }
            else if (p->balance == 0) { p->balance = -1; *grow = 1; }
            else {
                if (p->left->balance < 0) p = LL_Rotate_Insert(p);
                else p = LR_Rotate_Insert(p);
                *grow = 0;
            }
        }
    } else if (data > p->data) {
        p->right = insertAVL_Rec(p->right, data, grow);
        if (*grow) {
            if (p->balance < 0) { p->balance = 0; *grow = 0; }
            else if (p->balance == 0) { p->balance = 1; *grow = 1; }
            else {
                if (p->right->balance > 0) p = RR_Rotate_Insert(p);
                else p = RL_Rotate_Insert(p);
                *grow = 0;
            }
        }
    }
    return p;
}

Node* insertAVL(Node* root, int data) {
    int grow = 0;
    return insertAVL_Rec(root, data, &grow);
}


// Вспомогательная процедура del
// Удаляет узел, имеющий 2 поддерева, заменяя его на самый правый из левого поддерева
void del(Node** r, Node** q, int* decrease) {
    if ((*r)->right != NULL) {
        del(&((*r)->right), q, decrease);
        if (*decrease) BR(r, decrease); // Возврат из правого поддерева -> балансировка BR
    } else {
        (*q)->data = (*r)->data;
        *q = *r; // q теперь указывает на удаляемый узел
        *r = (*r)->left;
        *decrease = 1; // Уменьшение := ИСТИНА
    }
}

// Основная процедура удаления
void deleteAVL_Rec(Node** p, int x, int* decrease) {
    if (*p == NULL) {
        *decrease = 0; 
    } else if (x < (*p)->data) {
        deleteAVL_Rec(&((*p)->left), x, decrease);
        if (*decrease) BL(p, decrease); // Уменьшение в левом -> балансировка BL
    } else if (x > (*p)->data) {
        deleteAVL_Rec(&((*p)->right), x, decrease);
        if (*decrease) BR(p, decrease); // Уменьшение в правом -> балансировка BR
    } else {
        // Удаление вершины по адресу p
        Node* q = *p;
        if (q->left == NULL) {
            *p = q->right;
            *decrease = 1;
            free(q);
        } else if (q->right == NULL) {
            *p = q->left;
            *decrease = 1;
            free(q);
        } else {
            // Два поддерева. Вызов процедуры del
            del(&(q->left), &q, decrease);
            if (*decrease) BL(p, decrease); 
            free(q); 
        }
    }
}

Node* deleteAVL(Node* root, int key) {
    int decrease = 0;
    deleteAVL_Rec(&root, key, &decrease);
    return root;
}


void inorder(Node* root) {
    if (root) {
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

void shuffle(int* array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// --- MAIN ---
int main() {
    srand(time(NULL));
    int N = 100;
    int arr[100];
    for (int i = 0; i < N; i++) arr[i] = i + 1;
    shuffle(arr, N);

    Node* root = NULL;

    // Построение
    for (int i = 0; i < N; i++) {
        root = insertAVL(root, arr[i]);
    }

    printf("АВЛ-дерево построено (100 вершин).\n");
    printf("Статистика вставки: Поворотов = %d\n\n", g_insertRotations);
    
    PrintTree(root, 0);

    // Интерактивное удаление
    printf("=== Удаление 10 вершин ===\n");
    for (int i = 0; i < 10; i++) {
        int key;
        printf("Шаг %d/10. Введите число для удаления: ", i + 1);
        if (scanf("%d", &key) != 1) break;
        
        root = deleteAVL(root, key);
        
        printf("-> Дерево после удаления %d:\n", key);
        PrintTree(root, 0); 
        printf("Обход: ");
        inorder(root);
        printf("\n\n");
    }
    
    printf("\n=== Статистика по Заданию 4* ===\n");
    printf("Всего вставок: %d, поворотов: %d\n", N, g_insertRotations);
    printf("Всего удалений: 10, поворотов при удалении: %d\n", g_deleteRotations);

    return 0;
}