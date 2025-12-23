#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N 100

typedef struct Node {
    int data;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct AVLNode {
    int data;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;

} AVLNode;

Node* createNode(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

AVLNode* createAVLNode(int data) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

int getHeight(AVLNode* node) {
    if (!node) return 0;
    return node->height;
}

int getBalance(AVLNode* node) {
    if (!node) return 0;
    return getHeight(node->left) - getHeight(node->right);
}

int max(int a, int b) {
    return (a > b) ? a : b;
}


AVLNode* rightRotate(AVLNode* node) {
    AVLNode* temp = node->left;
    node->left = node->left->right;
    temp->right = node;
    node = temp;
    
    node->right->height = max(getHeight(node->right->left), getHeight(node->right->right)) + 1;
    node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

    return node;

}

AVLNode* leftRotate(AVLNode* node) {
    AVLNode* temp = node->right;
    node->right = node->right->left;
    temp->left = node;
    node = temp;

    node->left->height = max(getHeight(node->left->left), getHeight(node->left->right)) + 1;
    node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

    return node;
}


AVLNode* insertAVL(AVLNode* node, int data) {
    if (node == NULL) {
        node = createAVLNode(data);
        return node;
    }

    if (data < node->data) {
        node->left = insertAVL(node->left, data);
    } else if (node->data < data) {
        node->right = insertAVL(node->right, data);
    } else return node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    int balance = getBalance(node);

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

void printTree(AVLNode* root, int level) {
    if (root) {
        printTree(root->right, level + 1);
        printf("%*s%d\n", level * 4, "", root->data);
        printTree(root->left, level + 1);
    }
}

void inOrder(Node* node) {
    if (node) {
        inOrder(node->left);
        printf("'%d', ", node->data);
        inOrder(node->right);
    }
}

void inOrder(AVLNode* node) {
    if (node) {
        inOrder(node->left);
        printf("'%d', ", node->data);
        inOrder(node->right);
    }
}

void preOrder(Node* node) {
    if (node == NULL) {
        return;
    }

    printf("'%d' ", node->data);
    preOrder(node->left);
    preOrder(node->right);
}

void postOrder(Node* node) {
    if (node == NULL) {
        return;
    }

    postOrder(node->left);
    postOrder(node->right);
    printf("'%d' ", node->data);
}



void fillArr(int arr[], int n) {
    for (int i=0; i<n; i++) {
        arr[i] = i;
    }
}

void printArr(int arr[], int n) {
    printf("\n");
    for (int i=0; i<n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

Node* buildISDP(int arr[], int L, int R) {
    if (L > R) {
        return NULL;
    }

    int mid = (L + R)/2;
    Node* node = createNode(arr[mid]);
    node->left = buildISDP(arr, L, mid-1);
    node->right = buildISDP(arr, mid+1, R);
    
    return node;
}

Node* insertRecursive(Node* node, int data) {
    if (node == NULL) {
        node = createNode(data);
    }

    if (data < node->data) {
        node->left = insertRecursive(node->left, data);
    } else if (node->data < data) {
        node->right = insertRecursive(node->right, data);
    }
    return node;
}

void insertDoubleInderect(Node** root, int data) {
    Node** node = root;
    while (*node != NULL) {
        if (data < (*node)->data) {
            node = &((*node)->left);
        } else if (data > (*node)->data) {
            node = &((*node)->right);
        } else {
            return; // ignore duplicates
        }
    }
    (*node) = createNode(data);
}

int height(Node* node) {
    if (node == NULL) {
        return 0;
    }
    int lh = height(node->left);
    int rh = height(node->right);

    return 1 + (lh > rh ? lh : rh);
}

int checkSum(Node* node) {
    if (!node) return 0;

    return checkSum(node->left) + checkSum(node->right) + node->data;
}

void depthSum(Node* node, int depth, int* sum, int* count) {
    if (!node) return;

    (*sum) += depth;
    (*count)++;
    depthSum(node->left, depth+1, sum, count);
    depthSum(node->right, depth+1, sum, count);
}

Node* findFromBST(Node* node, int data) {
    if (!node) return NULL;

    if (data == node->data) {
        return node;
    } else if (data < node->data) {
        findFromBST(node->left, data);
    } else {
        findFromBST(node->right, data);
    }
    return NULL;
}

Node* findMin(Node* node) {
    while (node != NULL && node->left != NULL) {
        node = node->left;
    }
    return node;
}

Node* deleteFromBST(Node* node, int data) {
    if (!node) return NULL;

    if (data < node->data) {
        node->left = deleteFromBST(node->left, data);
    } else if (node->data < data){
        node->right = deleteFromBST(node->right, data);
    } else {
        if (node->right == NULL) {
            Node* temp = node->left;
            free(node);
            return temp;
        } else if (node->left == NULL) {
            Node* temp = node->right;
            free(node);
            return temp;
        }

        Node* temp = findMin(node->right);

        node->data = temp->data;

        node->right = deleteFromBST(node->right, temp->data);
    }
    return node;
}




double avgHeight(Node* node) {
    int sum = 0;
    int count = 0;

    depthSum(node, 1, &sum, &count);
    if (count == 0) return 0;
    return (double)(sum)/(count);
}

int size(Node* node) {
    if (!node) return 0;

    return 1 + size(node->left) + size(node->right);
}


int main() {
    //2 pbst
    // int arr[N];
    // fillArr(arr, N);
    // printArr(arr, N);


    // printf("\n");
    // Node* root = buildISDP(arr, 0, N-1);
    // printf("\n");
    // inOrder(root);
    
    // 3 rbst
    // int arr1[] = {3, 13, 55, 21, 2, 10, 4};
    // Node* node2 = NULL;

    // for (int i=0; i<7; i++) {
    //     node2 = insertRecursive(node2, arr1[i]);
    // }

    // printf("\n");
    // inOrder(node2);
    // printf("\n");
    // printf("%d", height(node2));
    // printf("\n");
    // printf("%d", checkSum(node2));

    // int depth = 0;
    // int sum = 0;
    // int count = 0;
    // depthSum(node2, 1, &sum, &count);
    // printf("\n");
    // printf("depthSum: %d", sum);
    // printf("\n");
    // printf("avgHeight: %.2f", avgHeight(node2));
    // printf("\n");

    // Node* nd = NULL;

    // nd = findFromBST(node2, 3);
    // if (nd) {
    //     printf("found: %d", nd->data);
    // } else {
    //     printf("not found");
    // }

    // deleteFromBST(node2, 33);
    
    // printf("\n");
    // inOrder(node2);
    // printf("\n");

    int arr1[] = {3, 13, 55, 21, 2, 10, 4, 6, 24};
    AVLNode* root = NULL;

    for (int i=0; i<9; i++) {
        root = insertAVL(root, arr1[i]);
    }

    printf("\n");
    inOrder(root);
    printf("\n");
    printf("\n");
    printTree(root, 0);

    return 0;
}