#include <stdio.h>
#include "kv.h"

int main() {
    kv_t *table = kv_init(1024);
    printf("Table pointer: %p \n", table);
    printf("Capacity: %zu\n", table->capacity);

    kv_put (table, "key1", "valueAltered");
    kv_put (table, "key2", "value2");
    kv_put (table, "key3", "value2");

    char *val = kv_get(table, "key1");
    char *val1 = kv_get(table, "key2");
    char *val2 = kv_get(table, "keyx");

    printf("%s %s %s\n", val, val1, val2);

    kv_delete(table, "key1");
    val = kv_get(table, "key1");
    printf("%s %s %s\n", val, val1, val2);
   
    return 0;
}
