#include <stdio.h>
#include "kv.h"

int main() {
    kv_t *table = kv_init(1024);
    printf("Table pointer: %p \n", table);
    printf("Capacity: %zu\n", table->capacity);

    kv_put (table, "key1", "valueAltered");
    kv_put (table, "key2", "value2");
    kv_put (table, "key3", "value2");

    for (int i = 0; i < table->capacity; i++) {
        kv_entry_t entry = table->entries[i];
        if (!entry.key) continue; // Skip empty entries
        printf("Entry [%d]: key=%s, value=%s\n", i, entry.key, entry.value);
    }
    return 0;
}
