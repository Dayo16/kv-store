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

int kv_put(kv_t* db, char* key, char* value ) {
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
            free(entry->value); 
            entry->value = newval;
            return 0;
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
            return 0;
        }
    }

    // If we reach here, it means the table is full
    return -2;
}

kv_t *kv_init(size_t capacity) {
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

// fn kv_get
// params:
// - db: a pointer to the db
// - key: a pointer to the key value
// returns: a pointer to the value, or NULL if the key doesn't exist
char *kv_get(kv_t* db, char* key){
    if(!db || !key) return NULL;

    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity - 1; i++) {
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_idx];
        
        // No key or Tombstone, return NULL
        if(entry->key == NULL) {
            return NULL;
        }

        // Find an entry and the keys match
        if (entry->key && 
            entry->key != (void*)TOMBSTONE &&
            !strcmp(entry->key, key)) {
            return entry->value;
        }


    }

    //no key found
    return NULL;
}

// fn kv_delete
// params:
// - db: a pointer to the db
// - key: a pointer to the key value
// returns: the index off the deleted value, -1 if not found
int kv_delete(kv_t* db, char* key){
    if (!db || !key) return -1;
    
    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity - 1; i++){
        size_t real_idx = (idx+i) % db->capacity;
        kv_entry_t *entry = &db->entries[real_idx];

        if (entry->key == NULL) return -1;

        if (entry->key && 
        entry->key != TOMBSTONE &&
        !strcmp(entry->key, key)) {
            free(entry->key);
            free(entry->value);
            db->count--;
            entry->key = TOMBSTONE;
            entry->value = NULL;

            return real_idx;
        }
    }
    
    return -1;
}

// fn kv_free
// params:
// - db: a pointer to the db
// returns: 0 on success, -1 on failure
int kv_free(kv_t* db) {
    if (!db) return -1;

    for (int i = 0; i < db->capacity - 1; i++) {
        kv_entry_t *entry = &db->entries[i];

        if (entry->key && entry->key != (void*)TOMBSTONE){
            free(entry->key);
            free(entry->value);
            entry->key = NULL;
            entry->value = NULL;
            db->count--;
        }
    }
    free(db->entries);
    free(db);

    return 0;
}
