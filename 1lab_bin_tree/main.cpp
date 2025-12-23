#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

// Функция создания нового узла
Node* createNode(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 1. Обход сверху вниз (Preorder: Корень -> Лево -> Право)
void preorder(Node* root) {
    if (root) {
        printf("%d ", root->data);
        preorder(root->left);
        preorder(root->right);
    }
}

// 2. Обход слева направо (Inorder: Лево -> Корень -> Право)
void inorder(Node* root) {
    if (root) {
        inorder(root->left);
        printf("%d ", root->data);
        inorder(root->right);
    }
}

// 3. Обход снизу вверх (Postorder: Лево -> Право -> Корень)
void postorder(Node* root) {
    if (root) {
        postorder(root->left);
        postorder(root->right);
        printf("%d ", root->data);
    }
}

// Размер дерева (количество узлов)
int size(Node* root) {
    if (!root) return 0;
    return 1 + size(root->left) + size(root->right);
}

// Контрольная сумма (сумма значений всех узлов)
int checksum(Node* root) {
    if (!root) return 0;
    return root->data + checksum(root->left) + checksum(root->right);
}

// Высота дерева
int height(Node* root) {
    if (!root) return 0;
    int lh = height(root->left);
    int rh = height(root->right);
    return 1 + (lh > rh ? lh : rh);
}

// Вспомогательная функция для средней высоты
void depthSum(Node* root, int depth, int* sum, int* count) {
    if (!root) return;
    *sum += depth;
    (*count)++;
    depthSum(root->left, depth + 1, sum, count);
    depthSum(root->right, depth + 1, sum, count);
}

// Средняя высота
double avgDepth(Node* root) {
    int sum = 0, count = 0;
    depthSum(root, 1, &sum, &count); // Считаем корень уровнем 1
    if (count == 0) return 0;
    return (double)sum / count;
}

int main() {
    // 1. Разместить дерево в памяти (Вариант 23)
    // Создаем 6 узлов с произвольными числами (от 1 до 20)
    Node* root = createNode(10); // Самый верхний (Корень)
    Node* n2 = createNode(5);    // Левый от корня
    Node* n3 = createNode(15);   // Правый от корня
    Node* n4 = createNode(12);   // Левый от узла n3
    Node* n5 = createNode(18);   // Правый от узла n3
    Node* n6 = createNode(8);    // Левый от узла n4

    // Строим связи согласно картинке 23:
    
    //       (10) <- root
    //       /  \
    //     (5)  (15) <- n3
    //          /  \
    //   n4 ->(12) (18)
    //        /
    //      (8)
    
    root->left = n2;
    root->right = n3;
    
    n3->left = n4;
    n3->right = n5;
    
    n4->left = n6;

    // 2. Вывод обходов
    printf("Обход сверху вниз (Preorder): ");
    preorder(root);
    printf("\n");

    printf("Обход слева направо (Inorder): ");
    inorder(root);
    printf("\n");

    printf("Обход снизу вверх (Postorder): ");
    postorder(root);
    printf("\n");

    printf("\n--------------------------\n");

    // 3. Характеристики дерева
    printf("Размер дерева: %d\n", size(root));       // Должно быть 6
    printf("Контрольная сумма: %d\n", checksum(root)); 
    printf("Высота дерева: %d\n", height(root));     // Должно быть 4 (10->15->12->8)
    printf("Средняя высота: %.2f\n", avgDepth(root));

    free(n6); free(n5); free(n4); free(n3); free(n2); free(root);
    
    return 0;
}