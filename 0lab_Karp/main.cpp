#include <stdio.h>
#include <string.h>
#include <math.h>

// 1. Метод прямого поиска (перебором)
int directSearch(const char* text, const char* pattern, int* comparisons) {
    int n = strlen(text);
    int m = strlen(pattern);
    int found = 0;
    *comparisons = 0;

    printf("Метод прямого поиска:\n");
    printf("Текст: %s\n", text);
    printf("Подстрока: %s\n", pattern);
    printf("Индексы вхождения: ");

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            (*comparisons)++;
            if (text[i + j] != pattern[j]) {
                break;
            }
        }
        
        if (j == m) {
            printf("%d ", i);
            found++;
        }
    }
    
    if (found == 0) {
        printf("не найдено");
    }
    printf("\nКоличество сравнений: %d\n\n", *comparisons);
    
    return found;
}

// 2. Метод Рабина-Карпа
int rabinKarpSearch(const char* text, const char* pattern, int* comparisons) {
    int n = strlen(text);
    int m = strlen(pattern);
    int found = 0;
    *comparisons = 0;

    const int prime = 101;   // простое число для модуля
    const int base = 256;    // размер алфавита (для ASCII)

    long long patternHash = 0;
    long long textHash = 0;
    long long h = 1; // значение base^(m-1) % prime
    

    // Вычисляем h = pow(base, m-1) % prime
    for (int i = 0; i < m - 1; i++)
        h = (h * base) % prime;

    // Начальное хеширование шаблона и первого окна текста
    for (int i = 0; i < m; i++) {
        patternHash = (base * patternHash + pattern[i]) % prime;
        textHash = (base * textHash + text[i]) % prime;
    }

    printf("Метод Рабина-Карпа:\n");
    printf("Текст: %s\n", text);
    printf("Подстрока: %s\n", pattern);
    printf("Индексы вхождения: ");

    // Сканируем текст
    for (int i = 0; i <= n - m; i++) {
        // Проверяем совпадение хешей
        if (patternHash == textHash) {
            int j;
            for (j = 0; j < m; j++) {
                (*comparisons)++;
                if (text[i + j] != pattern[j]) break;
            }
            if (j == m) {
                printf("%d ", i);
                found++;
            }
        }

        // Обновляем хеш для следующего окна
        if (i < n - m) {
            textHash = (base * (textHash - text[i] * h) + text[i + m]) % prime;

            // возможен отрицательный результат, приводим к положительному
            if (textHash < 0)
                textHash += prime;
        }
    }

    if (found == 0) {
        printf("не найдено");
    }
    printf("\nКоличество сравнений: %d\n\n", *comparisons);

    return found;
}

int main() {
    char text[1000];
    char pattern[100];
    int directComparisons, rkComparisons;
    
    printf("=== Поиск подстрок ===\n\n");
    
    printf("Введите текст: ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = '\0';
    
    printf("Введите подстроку для поиска: ");
    fgets(pattern, sizeof(pattern), stdin);
    pattern[strcspn(pattern, "\n")] = '\0';
    
    printf("\n");
    
    directSearch(text, pattern, &directComparisons);
    rabinKarpSearch(text, pattern, &rkComparisons);
    
    printf("3. Сравнение количества посимвольных сравнений:\n");
    printf("Метод прямого поиска: %d сравнений\n", directComparisons);
    printf("Метод Рабина-Карпа: %d сравнений\n", rkComparisons);
    
    if (directComparisons < rkComparisons) {
        printf("Метод прямого поиска эффективнее на %d сравнений\n", 
               rkComparisons - directComparisons);
    } else if (directComparisons > rkComparisons) {
        printf("Метод Рабина-Карпа эффективнее на %d сравнений\n", 
               directComparisons - rkComparisons);
    } else {
        printf("Оба метода показали одинаковую эффективность\n");
    }
    
    return 0;
}