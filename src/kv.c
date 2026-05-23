#include "kv.h"
#include <string.h>

// fn kv_put
// params:
//  - db: pointer to the db
//  - key: a pointer to the key value
//  - value: a pointer to the value itself
// returns: the index of the key, on error returns -1

size_t hash(const char* val, int capacity) {
    size_t hash = 0x13371337deadbeef;

    while(*val) {
        hash ^= *val;
        hash = hash << 8;
        hash += *val;

        val++;

    }
    return hash % capacity;
}

int kv_put(kv_t* db, const char* key, const char* value ) {
    if(!db || !key || !value) return -1;

    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity - 1; i++) {
        // Need a tombstone value to mark deleted entries
        // So i can delete entries without breaking the chain of collisions
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t* entry = &db->entries[real_idx];
        
        // If the key already exists, update the value
        if (entry->key &&
             entry->key != (void*)TOMBSTONE &&
             !strcmp(entry->key, key)
            ) {
            char* newval = strdup(value); // strdup allocates new memory for the value, so we can safely free the old value
            if (!newval) return -1; 
            entry->value = newval;
            return real_idx;
        }

        // If the key doesn't exist, insert a new entry
        if(!entry->key || entry->key == (void*)TOMBSTONE) {
            char* newval = strdup(value);
            char* newkey = strdup(key);
            if (!newval || !newkey) {
                free(newval);
                free(newkey);
                return -1;
            }
            entry->key = newkey;
            entry->value = newval;
            db->count++;
            return real_idx;
        }
    }

    // If we reach here, it means the table is full
    return -2;
}

kv_t* kv_init(size_t capacity) {
    if (capacity == 0 ) return NULL;

    kv_t* table = malloc(sizeof(kv_t));
    if (table == NULL) return NULL;

    table->capacity = capacity;
    table->count = 0;
    table->entries = calloc(sizeof(kv_entry_t), capacity);
    if (table->entries == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

char* kv_get(kv_t* db, const char* key){
    return NULL;
}

int kv_delete(kv_t* db, const char* key){
    return -1;
}

void kv_free(kv_t* db) {
}
