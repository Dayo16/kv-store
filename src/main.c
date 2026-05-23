#include <stdio.h>
#include "kv.h"

int main() {
    kv_t *table = kv_init(3);
    printf("%p \n", table);
    printf("Capacity: %zu\n", table->capacity);
    printf("Capacity: %ld\n", table->capacity);

    return 0;
}
