/*
 * File: LRU.c
 * @author: Doğukan Özdemir <dogu757@gmail.com>
 * Date: 14.01.2022
 */


#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// A doubly linked list struct node to hold the person's information
struct Node
{
    char *id;
    char *name;
    char *surname;


    struct Node *next;
    struct Node *prev;
};

// Pair struct to hold an entry in the map
// key: id value: node
typedef struct hashmap
{
    char *key;
    struct Node *node;
} Hashmap;


Hashmap *get(char *key, Hashmap **hashmap, int M);
void remove_hash(char *key, Hashmap **hashmap, int M);
void insert_hash(char *key, Hashmap **hashmap, Hashmap *pair, int M);
void put_on_top(struct Node **root, struct Node **tail, struct Node *newNode);



// root : a pointer to the root of the cache
// returns : the current size of the cache
int get_size(struct Node *root)
{
    int size = 0;
    while (root != NULL)
    {
        size++;
        root = root->next;
    }
    return size;
}

// Print the cache
// root : a pointer to the root of the cache
void print_cache(struct Node *root)
{
    printf("CACHE : \n");
    printf("--------------------------------------------------------------\n");
    printf("HEAD <--> ");
    while (root != NULL)
    {
        printf("%s <--> ", root->name);
        root = root->next;
    }
    printf("TAIL");
    printf("\n--------------------------------------------------------------\n\n");
}

// Delete a node from the cache
// root : a pointer to the root of the cache
// tail : a pointer to the tail of the cache
// rm   : a pointer to the node to be removed from the cache
void delete_node(struct Node **root, struct Node **tail, struct Node *rm)
{

    if (*root == NULL || rm == NULL)
    {
        return;
    }
    struct Node *prevnode = rm->prev;
    struct Node *nextnode = rm->next;

    if (prevnode != NULL)
    {
        prevnode->next = rm->next;
    }
    else
    {
        *root = nextnode;
    }

    if (nextnode != NULL)
    {
        nextnode->prev = prevnode;
    }
    else
    {
        *tail = prevnode;
    }
}

// Put a node into the cache
// map : a pointer to the hashmap
// root : a pointer to the root of the cache
// tail : a pointer to the tail of the cache
// node : a node formed to be added to the cache as an entry
// C : capacity of the cache
// M : capacity of the hashmap
void put_cache(Hashmap **map, struct Node **root, struct Node **tail, struct Node *node, int C, int M)
{

    // get the node from the hashmap with id.
    Hashmap *currentHash = get(node->id, map, M);

    // if this node already exists in the hashmap
    if (currentHash != NULL)
    {

        // replace the old information with the new
        currentHash->node->id = node->id;
        currentHash->node->name = node->name;
        currentHash->node->surname = node->surname;

        // delete that node
        delete_node(root, tail, currentHash->node);
        // and put it on top
        put_on_top(root, tail, currentHash->node);
    }
    else
    {
        // check if we are about exceed the cache length
        if (get_size(*root) >= C)
        {
            // if the cache length is about to be exceeded
            // remove the LEAST RECENTLY USED node from the cache and map
            remove_hash((*tail)->id, map, M);
            delete_node(root, tail, *tail);
        }
        // Allocate new memory for the new entry
        Hashmap *hash = (Hashmap *)malloc(sizeof(Hashmap));
        hash->key = node->id;
        hash->node = node;
        // insert it into map
        insert_hash(hash->key, map, hash, M);
        // and put that on top of the cache
        put_on_top(root, tail, node);
    }
}

// Put the given node parameter on top of the cache
// root    : a pointer to the root of the cache
// tail    : a pointer to the tail of the cache
// newNode : a pointer to the new node to be added to the top
void put_on_top(struct Node **root, struct Node **tail, struct Node *newNode)
{
    newNode->next = (*root);
    newNode->prev = NULL;

    if (*root != NULL)
        (*root)->prev = newNode;

    *root = newNode;

    if (*tail == NULL)
        *tail = newNode;
}

// Get node from the cache
// key     : the key of the requested node
// map     : a pointer to the hashmap array
// root    : a pointer to the root of the cache
// tail    : a pointer to the tail of the cache
// M       : capacity of the hashmap
// returns : the requested cache
struct Node* get_cache(char *key, Hashmap **map, struct Node **root, struct Node **tail, int M)
{
    Hashmap *currentHash = get(key, map, M);
    printf("\nGetting the information with id: %s\n", key);
    if (currentHash != NULL)
    {

        printf("id      : %s\n", currentHash->node->id);
        printf("name    : %s\n", currentHash->node->name);
        printf("surname : %s\n", currentHash->node->surname);

        delete_node(root, tail, currentHash->node);
        put_on_top(root, tail, currentHash->node);
        return currentHash->node;
    }
    else
    {
        printf("A record with this key does not exist!\n");
        return NULL;
    }
    printf("\n\n");
}


// initialize the hashmap and set every entry to NULL
// hashmap : a pointer to the hashmap array
void init_table(Hashmap **hashmap, int M)
{
    int i;
    for (i = 0; i < M; i++)
    {
        hashmap[i] = NULL;
    }
}

// Convert a key to a value integer using horner's rule
// key     : key to convert to horner integer
// returns : a key converted to an integer
int horner(char *key)
{
    // a dummy prime var
    const int P = 23;
    int horner = 0;
    int i;
    for (i = 0; i < strlen(key); i++)
    {
        // summing the ascii values of the key and multipliying with P
        horner = P * horner + (int)key[i];
    }
    return horner;
}

// A hash function
// key : key to hash
// M : capacity of the Hashmap
int hash_one(char *key, int M)
{
    return horner(key) % M;
}

// A hash function
// key : key to hash
// M : capacity of the Hashmap
int hash_two(char *key, int M)
{
    return 1 + (horner(key) % (M - 1));
}

// Insert an entry into the hashmap
// key : key of the entry
// hashmap : a pointer to the hashmap array
// pair : a pointer to (key,node) entry to add to the hashmap
// M : capacity of the Hashmap
void insert_hash(char *key, Hashmap **hashmap, Hashmap *pair, int M)
{

    int hash1 = hash_one(key, M);
    int index = hash1;

    // if this index is occupied
    if (hashmap[index] != NULL)
    {
        int index2 = hash_two(key, M);
        int i = 0;
        // double hashing because first index is occupied
        // continue this loop until an empty place has been found
        while (1)
        {
            int newIndex = (index + i * index2) % M;
            if (hashmap[newIndex] == NULL)
            {
                hashmap[newIndex] = pair;
                break;
            }
            i++;
        }
    }
    // if its not occupied just place it at the index given from hash1
    else
    {
        hashmap[index] = pair;
    }
}

// Get an entry from the hashmap
// key     : key of the requested entry
// M       : capacity of the hashmap
// returns : the requested entry from the hashmap
Hashmap *get(char *key, Hashmap **hashmap, int M)
{
    int hash1 = hash_one(key, M);
    int hash2 = hash_two(key, M);
    int i = 0;

    // Same working logic as insert hash but this time key is given
    // until our key matches the one in the currentindex
    // if it hasnt been found, continue double hashing until it is.
    // this time return it.

    // continue this loop until an the key of the entry matches the key given
    while (1)
    {
        Hashmap *currentPair = hashmap[(hash1 + i * hash2) % M];
        if (currentPair == NULL)
        {
            return NULL;
        }
        if (!strcmp(currentPair->key, key))
        {
            return currentPair;
        }
        i++;
    }
}

// Remove an entry from the hashmap
// key : key of the requested entry
// M : capacity of the hashmap
void remove_hash(char *key, Hashmap **hashmap, int M)
{
    int hash1 = hash_one(key, M);
    int hash2 = hash_two(key, M);
    int i = 0;
    // Same working logic as get,
    // this time set the found hash to NULL to free it
    while (1)
    {
        Hashmap *currentPair = hashmap[(hash1 + i * hash2) % M];
        if (currentPair == NULL)
        {
            return;
        }
        if (!strcmp(currentPair->key, key))
        {
            hashmap[(hash1 + i * hash2) % M] = NULL;
        }
        i++;
    }
}

// Print the Hashmap
// hashmap : a pointer to the Hashmap
// M : capacity of the hashmap
void print_hashmap(Hashmap **hashmap, int M)
{
    printf("HASHMAP : \n");
    printf("------------------\n");
    int i;
    for (i = 0; i < M; i++)
    {
        printf("---------------\n");
        printf("| key | value | \n");
        printf("---------------\n");
        if (hashmap[i] == NULL)
        {

            printf("| %s | %s |\n", "NULL", "NULL");
        }
        else
        {
            printf("| %s | %s |\n", hashmap[i]->key, hashmap[i]->node->name);
        }
    }
    printf("------------------\n");
}
// Check if n is a prime
int check_prime(int n)
{
    int flag = 1;
    int i;
    for (i = 2; i <= sqrt(n); i++)
    {
        if (n % i == 0)
        {
            flag = 0;
            break;
        }
    }
    if (n <= 1)
        flag = 0;

    return flag;
}

// Get the next prime >= n
int get_next_prime(int n)
{
    if (check_prime(n))
    {
        return n;
    }
    else
    {
        return get_next_prime(n + 1);
    }
}

/*
Example Usage:

With a cach length of 4, adding 5 items to the cache and retrieve 2 of them.
Here, you can use the 'print_hashmap' and 'print_cache' functions
To see how each actipn affects the cache and the hashmap.
*/

int main()
{

    // sample values for loadfactor, cache length and hashmap capacity (M),
    // change it for your needs
    const float LOAD_FACTOR = 0.6;
    const int CACHE_LENGTH = 4;
    int M;

    struct Node *root = NULL;
    struct Node *tail = NULL;

    M = get_next_prime(CACHE_LENGTH) / LOAD_FACTOR;

    Hashmap *hashmap[M];
    init_table(hashmap, M);

    struct Node *item = (struct Node *)malloc(sizeof(struct Node));
    item->id = "55612";
    item->name = "Steve";
    item->surname = "Rogers";
    put_cache(hashmap, &root, &tail, item, CACHE_LENGTH, M);


    struct Node *item2 = (struct Node *)malloc(sizeof(struct Node));
    item2->id = "14462";
    item2->name = "Natasha";
    item2->surname = "Romanoff";
    put_cache(hashmap, &root, &tail, item2, CACHE_LENGTH, M);

    struct Node *item3 = (struct Node *)malloc(sizeof(struct Node));
    item3->id = "00101";
    item3->name = "Bruce";
    item3->surname = "Wayne";
    put_cache(hashmap, &root, &tail, item3, CACHE_LENGTH, M);

    struct Node *item4 = (struct Node *)malloc(sizeof(struct Node));
    item4->id = "18912";
    item4->name = "Clark";
    item4->surname = "Kent";
    put_cache(hashmap, &root, &tail, item4, CACHE_LENGTH, M);

    struct Node *item5 = (struct Node *)malloc(sizeof(struct Node));
    item5->id = "66666";
    item5->name = "Peter";
    item5->surname = "Parker";
    put_cache(hashmap, &root, &tail, item5, CACHE_LENGTH, M);

    get_cache("18912", hashmap, &root, &tail, M);
    get_cache("66666", hashmap, &root, &tail, M);

    print_cache(root);

    return 0;
}
