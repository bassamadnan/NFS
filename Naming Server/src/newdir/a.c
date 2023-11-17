#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CACHE_SIZE 5
// Define a node for the linked list
typedef struct CacheNode {
    struct CacheEntry {
        char filename[1024];  
        int serverID;
    } entry;
    struct CacheNode* next;
    struct CacheNode* prev;
} CacheNode;

// Define a structure to represent the LRU cache
typedef struct LRUCache {
    CacheNode* head;
    CacheNode* tail;
    int size;
    int capacity;
} LRUCache;

// Initialize the LRU cache
LRUCache* createCache(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
    cache->capacity = capacity;
    return cache;
}

LRUCache* cache;
// Add a file to the cache
void addFile(LRUCache* cache, const char* filename, int serverID) {
    // Create a new cache node
    CacheNode* newNode = (CacheNode*)malloc(sizeof(CacheNode));
    strncpy(newNode->entry.filename, filename, sizeof(newNode->entry.filename));
    newNode->entry.serverID = serverID;
    newNode->prev = NULL;
    newNode->next = cache->head;

    if (cache->head) {
        cache->head->prev = newNode;
    }

    cache->head = newNode;

    if (!cache->tail) {
        cache->tail = newNode;
    }

    cache->size++;

    // If the cache is full, remove the least recently used file
    if (cache->size > cache->capacity) {
        CacheNode* lastNode = cache->tail;
        cache->tail = lastNode->prev;
        if (cache->tail) {
            cache->tail->next = NULL;
        }
        free(lastNode);
        cache->size--;
    }
}

// Retrieve a cache entry from the cache based on file name
struct CacheEntry retrieveEntry(LRUCache* cache, const char* filename) {
    // Search for the file in the cache
    CacheNode* currentNode = cache->head;
    while (currentNode) {
        if (strcmp(currentNode->entry.filename, filename) == 0) {
            // Move the accessed file to the front of the list
            if (currentNode != cache->head) {
                if (currentNode == cache->tail) {
                    cache->tail = currentNode->prev;
                }
                if (currentNode->prev) {
                    currentNode->prev->next = currentNode->next;
                }
                if (currentNode->next) {
                    currentNode->next->prev = currentNode->prev;
                }
                currentNode->prev = NULL;
                currentNode->next = cache->head;
                cache->head->prev = currentNode;
                cache->head = currentNode;
            }
            return currentNode->entry;
        }
        currentNode = currentNode->next;
    }

    // File not found in the cache
    struct CacheEntry notFoundEntry;
    notFoundEntry.serverID = -1; // Use -1 to indicate that the file was not found
    return notFoundEntry;
}

void printCache(LRUCache* cache) {
    printf("Current files in the cache:\n");
    CacheNode* currentNode = cache->head;
    while (currentNode) {
        printf("File: %s, Storage Server ID: %d\n", currentNode->entry.filename, currentNode->entry.serverID);
        currentNode = currentNode->next;
    }
}

int main() {
    cache = createCache(CACHE_SIZE);

    struct CacheEntry entry;

    // Add files to the cache
    addFile(cache, "file1.txt", 1); // Specify the storage server ID
    addFile(cache, "file2.txt", 2); // Specify the storage server ID
    addFile(cache, "file3.txt", 3); // Specify the storage server ID
    entry = retrieveEntry(cache, "file1.txt");
    printCache(cache);
    addFile(cache, "file4.txt", 1); // Specify the storage server ID
    addFile(cache, "file5.txt", 2); // Specify the storage server ID
    printCache(cache);
    addFile(cache, "file6.txt", 3); // Specify the storage server ID
    entry = retrieveEntry(cache, "file7.txt");
    printCache(cache);

    
    if (entry.serverID != -1) {
        printf("File found: File: %s, Storage Server ID: %d\n", entry.filename, entry.serverID);
    } else {
        printf("File not found\n");
    }

    return 0;
}
