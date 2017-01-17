#include <stdio.h>
#include <string.h>

typedef int (*comparator_t)(void*, void*);

void bubble_sort(void *items, size_t size, size_t count, comparator_t compare) {
    int i, j;
    char temp[size];
    for(i = 0; i < count - 1; ++i) {
        for(j = i + 1; j < count; ++j) {
            if (compare(items + (i * size), items + (j * size)) == 1) {
                memcpy(temp, items + (i * size), size);
                memcpy(items + i * size, items + (j * size), size);
                memcpy(items + (j * size), temp, size);
            }
        }
    }
}

int compare_long(void* p_a, void* p_b) {
    long a = *(long*)p_a;
    long b = *(long*)p_b;

    if(a < b) return -1;
    if(a > b) return 1;
    return 0;
}

int main(int argc, char *argv[]) {
    long x[] = { 5, 6, 1, 2, 9 };
    int i;

    for (i = 0; i < 5; ++i) printf("%ld ", x[i]);
    printf("\n");

    bubble_sort(x, sizeof(long), 5, compare_long);

    for (i = 0; i < 5; ++i) printf("%ld ", x[i]);
}
