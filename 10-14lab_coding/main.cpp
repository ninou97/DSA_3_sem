#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>

#define MAX_SYMBOLS 1024
#define MAX_CODE_LEN 128

// --- СТРУКТУРЫ ---

typedef struct {
    wchar_t symbol;
    long double prob;
    long long freq;
    int code_len;
    wchar_t code[MAX_CODE_LEN];
} Symbol;

typedef struct TrieNode {
    struct TrieNode *left;
    struct TrieNode *right;
    wchar_t symbol;
    int is_leaf;
} TrieNode;

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

// Сортировка по вероятности (убывание)
int compare_prob(const void *a, const void *b) {
    long double diff = ((Symbol*)b)->prob - ((Symbol*)a)->prob;
    if (diff > 0) return 1;
    if (diff < 0) return -1;
    return 0;
}

// Сортировка по алфавиту (для Гилберта-Мура)
int compare_alpha(const void *a, const void *b) {
    return ((Symbol*)a)->symbol - ((Symbol*)b)->symbol;
}

// --- ГРАФИКА ДЕРЕВА ---

TrieNode* createTrieNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    node->left = NULL; node->right = NULL;
    node->is_leaf = 0; node->symbol = 0;
    return node;
}

void insertTrie(TrieNode* root, Symbol* s) {
    TrieNode* curr = root;
    for (int i = 0; i < s->code_len; i++) {
        if (s->code[i] == L'0') {
            if (!curr->left) curr->left = createTrieNode();
            curr = curr->left;
        } else {
            if (!curr->right) curr->right = createTrieNode();
            curr = curr->right;
        }
    }
    curr->is_leaf = 1;
    curr->symbol = s->symbol;
}

void printTrieRecursive(TrieNode* node, int level, int isRight) {
    if (!node) return;
    printTrieRecursive(node->right, level + 1, 1);
    for (int i = 0; i < level; i++) wprintf(L"     ");
    if (level > 0) {
        if (isRight) wprintf(L"/---- ");
        else         wprintf(L"\\---- ");
    }
    if (node->is_leaf) {
        if (node->symbol == L'\n') wprintf(L"(\\n)\n");
        else if (node->symbol == L' ') wprintf(L"(sp)\n");
        else wprintf(L"%lc\n", node->symbol);
    } else {
        wprintf(L"*\n");
    }
    printTrieRecursive(node->left, level + 1, 0);
}

void freeTrie(TrieNode* node) {
    if (!node) return;
    freeTrie(node->left);
    freeTrie(node->right);
    free(node);
}

// --- ВЫВОД РЕЗУЛЬТАТОВ ---
void print_full_report(Symbol symbols[], int n, wchar_t* source_text, int text_len, long double entropy, const wchar_t* alg_name) {
    wprintf(L"\n======================= Код %ls (Таблица) =======================\n", alg_name);
    wprintf(L"%-8ls %-10ls %-12ls %-8ls %-20ls\n", L"Символ", L"Частота", L"Вероятность", L"Длина", L"Кодовое слово");
    wprintf(L"--------------------------------------------------------------------\n");
    
    for (int i = 0; i < n; i++) {
        wchar_t display_sym[10];
        if (symbols[i].symbol == L'\n') wcscpy(display_sym, L"\\n");
        else if (symbols[i].symbol == L' ') wcscpy(display_sym, L"SPACE");
        else swprintf(display_sym, 10, L"%lc", symbols[i].symbol);

        wprintf(L"%-8ls %-10lld %-12.6Lf %-8d %-20ls\n",
                display_sym, symbols[i].freq, symbols[i].prob, symbols[i].code_len, symbols[i].code);
    }

    long double kraft = 0;
    long double L_avg = 0;
    for (int i = 0; i < n; i++) {
        kraft += powl(2.0L, -symbols[i].code_len);
        L_avg += symbols[i].prob * symbols[i].code_len;
    }
    long double R = L_avg - entropy;

    wprintf(L"\n======================= Характеристики =======================\n");
    wprintf(L"| %-22ls | %-22ls | %-26ls | %-22ls |\n", L"Неравенство Крафта", L"Энтропия H", L"Средняя длина L", L"Избыточность R");
    wprintf(L"| %-22.6Lf | %-22.6Lf | %-26.6Lf | %-22.6Lf |\n", kraft, entropy, L_avg, R);
    if (kraft <= 1.000001) wprintf(L"Результат: Неравенство Крафта выполняется (<= 1).\n");
    else wprintf(L"Результат: Неравенство Крафта НЕ выполняется (%.6Lf > 1).\n", kraft);

    wprintf(L"\nЗакодированная последовательность (первые 100 символов исходного):\n");
    long long encoded_bits = 0;
    for (int i = 0; i < text_len; i++) {
        for (int j = 0; j < n; j++) {
            if (symbols[j].symbol == source_text[i]) {
                wprintf(L"%ls", symbols[j].code);
                encoded_bits += symbols[j].code_len;
                break;
            }
        }
    }
    wprintf(L"\n\n");
    
    TrieNode* root = createTrieNode();
    for(int i=0; i<n; i++) insertTrie(root, &symbols[i]);
    // printTrieRecursive(root, 0, 0); 
    freeTrie(root);
    wprintf(L"\n\n");
}

// === АЛГОРИТМЫ КОДИРОВАНИЯ ===

// 1. ШЕННОН
void shannon_coding(Symbol symbols[], int n) {
    double Q[MAX_SYMBOLS]; // Кумулятивная вероятность (нарастающая сумма)
    Q[0] = 0.0;
    // 2. Составим нарастающие суммы Q (стр. 99)
    for (int i = 0; i < n; i++) {
        if (i > 0) Q[i] = Q[i-1] + symbols[i-1].prob;
        
        // 3. Длина кодового слова
        symbols[i].code_len = (int)ceil(-log2(symbols[i].prob));
        
        // Перевод дробной части Q[i] в двоичный код
        double temp_Q = Q[i];
        for (int j = 0; j < symbols[i].code_len; j++) {
            temp_Q *= 2;
            int bit = (int)floor(temp_Q);
            symbols[i].code[j] = bit ? L'1' : L'0';
            if (temp_Q >= 1.0) temp_Q -= 1.0;
        }
        symbols[i].code[symbols[i].code_len] = L'\0';
    }
}

// 2. ФАНО

// Находит медиану (индекс разбиения)
int Med_PDF(Symbol symbols[], int L, int R) {
    long double SL = 0.0;
    // Сумма элементов первой части (предварительно от L до R-1)
    for (int i = L; i < R; i++) {
        SL += symbols[i].prob;
    }
    
    long double SR = symbols[R].prob; // Сумма второй части (пока только последний элемент)
    int m = R;
    
    // Сдвигаем границу влево, пока левая часть больше или равна правой
    while (SL >= SR) {
        m = m - 1;
        SL = SL - symbols[m].prob;
        SR = SR + symbols[m].prob;
    }
    
    return m;
}

void Fano_coding(Symbol symbols[], int L, int R, int k) {
    if (L < R) {
        // Деление массива вероятностей 
        int m = Med_PDF(symbols, L, R);
        
        // Приписываем 0 первой части, 1 второй части
        for (int i = L; i <= R; i++) {
            if (i <= m) {
                symbols[i].code[k] = L'0';
                symbols[i].code_len++;
            } else {
                symbols[i].code[k] = L'1';
                symbols[i].code_len++;
            }
        }
        Fano_coding(symbols, L, m, k + 1);
        Fano_coding(symbols, m + 1, R, k + 1);
    } else if (L == R) {
        symbols[L].code[k] = L'\0';
    }
}

// 3. ХАФФМАН 

// Функция Up (вставка суммы с сохранением порядка)
int Up(int n, long double q, long double P[]) {
    int i = n - 1;
    // Сдвигаем элементы вправо, освобождая место для q
    while (i >= 1 && P[i - 1] <= q) {
        P[i] = P[i - 1]; 
        i--;
    }
    P[i] = q;
    return i;
}

// Функция Down (формирование кодов)
void Down(int n, int j, wchar_t C[][MAX_CODE_LEN], int L[]) {
    wchar_t S[MAX_CODE_LEN];
    wcscpy(S, C[j]);
    int L_var = L[j];
    
    // Сдвиг вверх (восстановление пропущенных индексов)
    for (int i = j; i < n - 1; i++) {
        wcscpy(C[i], C[i + 1]);
        L[i] = L[i + 1];
    }
    
    // Восстановление префиксов для двух исходных элементов
    wcscpy(C[n - 2], S);
    wcscpy(C[n - 1], S);
    
    // Добавление 0 и 1
    int len = L_var;
    C[n - 2][len] = L'0'; C[n - 2][len+1] = L'\0';
    C[n - 1][len] = L'1'; C[n - 1][len+1] = L'\0';
    L[n - 2] = len + 1;
    L[n - 1] = len + 1;
}

void Huffman_Recursive(int n, long double P[], wchar_t C[][MAX_CODE_LEN], int L[]) {
    if (n == 2) {
        // Базовый случай: два символа 0 и 1
        C[0][0] = L'0'; C[0][1] = L'\0'; L[0] = 1;
        C[1][0] = L'1'; C[1][1] = L'\0'; L[1] = 1;
    } else {
        // Складываем две наименьшие вероятности (последние в отсортированном массиве)
        long double q = P[n - 2] + P[n - 1]; 
        int j = Up(n - 1, q, P); 
        
        Huffman_Recursive(n - 1, P, C, L);
        
        Down(n, j, C, L);
    }
}

// 4. ГИЛБЕРТ-МУР 
// Алфавитный код
void GilbertMoore(Symbol symbols[], int n) {
    // P - вероятности, Q - кумулятивные суммы (нарастающие)
    double P[MAX_SYMBOLS], Q[MAX_SYMBOLS], Pr[MAX_SYMBOLS];

    for(int i=0; i<n; i++) P[i] = symbols[i].prob;

    // 1. Составим суммы Qi 
    Q[0] = 0.0;
    for(int i=1; i<=n; i++) {
        Q[i] = Q[i-1] + P[i-1];
    }
    
    for(int i=0; i<n; i++) {
        // Середина интервала вероятности для i-го символа
        Pr[i] = Q[i] + P[i]/2.0; 

        // 3. Длина кода + 1
        symbols[i].code_len = (int)ceil(-log2(P[i])) + 1; 

        // Бинарное представление
        double temp = Pr[i];
        for(int j=0; j<symbols[i].code_len; j++) {
            temp *= 2;
            int bit = (int)floor(temp);
            symbols[i].code[j] = bit ? L'1' : L'0';
            if(temp >= 1.0) temp -= 1.0;
        }
        symbols[i].code[symbols[i].code_len] = L'\0';
    }
}

// === MAIN ===

int main(void) {
    setlocale(LC_ALL, ""); 
    
    FILE *f = fopen("text.txt", "r");
    if (!f) {
        wprintf(L"Ошибка: Файл text.txt не найден. Создайте файл с текстом.\n");
        return 1;
    }

    // Сбор статистики
    struct { wchar_t sym; long long freq; } freq_table[MAX_SYMBOLS];
    int freq_count = 0;
    long long total_symbols = 0;
    wchar_t source_text[105];
    int text_captured = 0;
    wint_t c;

    while ((c = fgetwc(f)) != WEOF) {
        if (c < 32 && c != L'\n') continue; // Пропуск управляющих символов кроме переноса
        total_symbols++;
        if (text_captured < 100) source_text[text_captured++] = c;

        int found = 0;
        for (int i = 0; i < freq_count; i++) {
            if (freq_table[i].sym == c) {
                freq_table[i].freq++;
                found = 1;
                break;
            }
        }
        if (!found) {
            freq_table[freq_count].sym = c;
            freq_table[freq_count].freq = 1;
            freq_count++;
        }
    }
    source_text[text_captured] = L'\0';
    fclose(f);

    if (total_symbols == 0) return 1;

    Symbol base[MAX_SYMBOLS];
    long double entropy = 0.0;
    for (int i = 0; i < freq_count; i++) {
        base[i].symbol = freq_table[i].sym;
        base[i].freq = freq_table[i].freq;
        base[i].prob = (long double)base[i].freq / total_symbols;
        base[i].code_len = 0;
        base[i].code[0] = L'\0';
        entropy -= base[i].prob * log2l(base[i].prob);
    }

    // Подготовка массивов (сортировка по вероятности для всех кроме Гилберта-Мура)
    Symbol prob_sorted[MAX_SYMBOLS];
    memcpy(prob_sorted, base, sizeof(Symbol) * freq_count);
    qsort(prob_sorted, freq_count, sizeof(Symbol), compare_prob);

    // Сортировка по алфавиту для Гилберта-Мура
    Symbol alpha_sorted[MAX_SYMBOLS];
    memcpy(alpha_sorted, base, sizeof(Symbol) * freq_count);
    qsort(alpha_sorted, freq_count, sizeof(Symbol), compare_alpha);

    // --- 1. ШЕННОН ---
    Symbol shannon[MAX_SYMBOLS];
    memcpy(shannon, prob_sorted, sizeof(Symbol) * freq_count);
    shannon_coding(shannon, freq_count);
    print_full_report(shannon, freq_count, source_text, text_captured, entropy, L"Шеннона");

    // --- 2. ФАНО ---
    Symbol fano[MAX_SYMBOLS];
    memcpy(fano, prob_sorted, sizeof(Symbol) * freq_count);
    Fano_coding(fano, 0, freq_count - 1, 0); // Используем обновленный Med_PDF внутри
    print_full_report(fano, freq_count, source_text, text_captured, entropy, L"Фано");

    // --- 3. ХАФФМАН ---
    Symbol huff[MAX_SYMBOLS];
    memcpy(huff, prob_sorted, sizeof(Symbol) * freq_count);
    long double P_arr[MAX_SYMBOLS];
    wchar_t C_arr[MAX_SYMBOLS][MAX_CODE_LEN];
    int L_arr[MAX_SYMBOLS];
    for(int i=0; i<freq_count; i++) P_arr[i] = huff[i].prob;
    
    Huffman_Recursive(freq_count, P_arr, C_arr, L_arr);
    
    for(int i=0; i<freq_count; i++) {
        wcscpy(huff[i].code, C_arr[i]);
        huff[i].code_len = L_arr[i];
    }
    print_full_report(huff, freq_count, source_text, text_captured, entropy, L"Хаффмана");

    // --- 4. ГИЛБЕРТ-МУР ---
    Symbol gm[MAX_SYMBOLS];
    memcpy(gm, alpha_sorted, sizeof(Symbol) * freq_count);
    GilbertMoore(gm, freq_count);
    
    // Для красивого вывода сортируем результат Гилберта-Мура по убыванию вероятности (как остальные)
    Symbol gm_display[MAX_SYMBOLS];
    memcpy(gm_display, gm, sizeof(Symbol) * freq_count);
    qsort(gm_display, freq_count, sizeof(Symbol), compare_prob);
    
    print_full_report(gm_display, freq_count, source_text, text_captured, entropy, L"Гилберта-Мура");

    return 0;
}