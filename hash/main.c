#include <stdio.h>
#include "HashMap.h"

int print_key_value(any_t key, any_t value) {
    printf("Key: %d, Value: %s\n", *((int*)key), (char*)value);
    return MAP_OK;
}

int main() {
    map_t myMap = hashmap_new();
    if (myMap == NULL) {
        fprintf(stderr, "Failed to create a new hashmap.\n");
        return 1;
    }

    int key1 = 42;
    const char* value1 = "ooga booga";
    hashmap_put(myMap, key1, value1);

    int key2 = 17;
    const char* value2 = "booga ooga";
    hashmap_put(myMap, key2, value2);

    int retrieve_key = 42;
    any_t retrieved_value;
    if (hashmap_get(myMap, retrieve_key, &retrieved_value) == MAP_OK) {
        printf("Value associated with key %d is %s\n", retrieve_key, (char*)retrieved_value);
    } else {
        printf("Key %d not found in the hashmap.\n", retrieve_key);
    }

    int remove_key = 17;
    if (hashmap_remove(myMap, remove_key) == MAP_OK) {
        printf("Key %d has been removed from the hashmap.\n", remove_key);
    } else {
        printf("Key %d not found in the hashmap.\n", remove_key);
    }

    //hashmap_iterate(myMap, print_key_value, NULL);

    hashmap_free(myMap);

    return 0;
}
