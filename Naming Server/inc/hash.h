#ifndef HASH_H
#define HASH_H

/* 
   This header file defines a basic hash map data structure and its associated
   functions for inserting, finding, and freeing key-value pairs.

   MAP_SIZE: The maximum size of the hash map.

   Data Structures:

   - struct KeyValue: This structure represents a key-value pair. It contains a key (as a string), a value (as a void pointer), and pointers to left and right children for AVL tree purposes. It also stores the height of the node in the AVL tree.

   - struct HashMap: This structure represents the hash map itself. It contains an array of pointers to struct KeyValue, with each slot in the array serving as a separate slot for storing key-value pairs.

   Hashing:
   The hash function takes a string key and converts it into an index within the hash map. This is done by iteratively applying a hash function to each character in the key. The final index is determined by taking the modulo of the result by MAP_SIZE.

   Initialization:
   initHashMap initializes the hash map by setting all slots in the array to NULL.

   Insertion:

   - insert inserts a key-value pair into the hash map. It first calculates the index using the hash function, and then it uses the insertNode function to insert the pair into the AVL tree at that index.

   - insertNode performs AVL tree insertion and self-balancing as needed. It keeps track of the balance factor to ensure that the tree remains balanced. If the balance factor is violated, it performs rotations (either right or left) to restore balance.

   Retrieval:

   - find retrieves a value by key in the hash map. It calculates the index using the hash function and then searches for the key in the AVL tree at that index using the search function.

   - search is a helper function that recursively searches for a key within an AVL tree.

   Memory Management:

   - The createNode function allocates memory for a new KeyValue structure, including memory for the key (duplicated with strdup), and initializes the fields.

   - freeTree is a recursive function used to free the memory associated with an AVL tree. It traverses the tree and frees the nodes and their keys.

   - freeHashMap is used to free the memory associated with the entire hash map. It calls freeTree for each slot in the array.

   Rotation Functions:
   - rightRotate and leftRotate are responsible for performing right and left rotations, respectively, in the AVL tree to maintain balance.

   Height and Balance Factor:

   - The height function calculates the height of a node in the AVL tree.

   - The getBalance function computes the balance factor of a node by subtracting the height of the right subtree from the height of the left subtree.

*/

#define MAP_SIZE 100

struct HashMap;

/* Initialize the hash map */
void initHashMap(struct HashMap* map);

/* Insert a key-value pair into the hash map */
void insert(struct HashMap* map, const char* key, void* value);

/* Find and retrieve a value by key in the hash map */
void* find(struct HashMap* map, const char* key);

/* Free the memory used by the hash map */
void freeHashMap(struct HashMap* map);

/* Hash function to convert a string key to an index in the hash map */
unsigned int hash(const char* key);

#endif

