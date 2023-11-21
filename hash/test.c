#include <stdio.h>
#include "hash.h"
struct HashMap {
    struct KeyValue* table[MAP_SIZE];
};

int main() {
    struct HashMap map;
    initHashMap(&map);

    insert(&map, "key1", (void*)10);
    insert(&map, "key2", (void*)20);

    void* value1 = find(&map, "key1");
    void* value2 = find(&map, "key2");

    printf("Value for key1: %d\n", (int)value1);
    printf("Value for key2: %d\n", (int)value2);

    void* value3 = find(&map, "key3");
    if (value3 == NULL) {
        printf("Key3 not found in the map.\n");
    }

    freeHashMap(&map);

    return 0;
}

