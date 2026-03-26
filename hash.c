#include "hash.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define HASH_ARRAY_SIZE 1048576

size_t accumulate_array(char *arr)
{
    size_t result = 0;

    for (size_t i = 0; arr[i] != '\0'; ++i)
    {
        result += (int)arr[i];
    }

    return result;
}

size_t hashing(char *str)
{
    return (strlen(str) + accumulate_array(str)) % HASH_ARRAY_SIZE;
}

Node** init_hash()
{
    Node **hash = (Node **)calloc(HASH_ARRAY_SIZE, sizeof(Node *));

    if (!hash)
    {
        perror("malloc");
        exit(errno);
    }

    return hash;
}

void free_node(Node* node)
{
    free(node->data);
    free(node->key);
    free(node);
}

void free_hash(Node **hash)
{
    for (size_t i = 0; i < HASH_ARRAY_SIZE; ++i)
    {
        while (hash[i])
        {
            Node *next = hash[i]->next;
            free_node(hash[i]);
            hash[i] = next;
        }
    }
    free(hash);
}

void set_value(Node** hash, char* key, char *value)
{
    size_t idx = hashing(key);

    Node *node = (Node *)malloc(sizeof(Node));

    node->key = strdup(key);
    node->data = strdup(value);
    node->data_length = strlen(node->data);
    node->next = NULL;

    if (hash[idx] == NULL)
    {
        hash[idx] = node;
        return;
    }

    Node* next = hash[idx];

    while(next->next)
    {
        if (strcmp(next->key, node->key) == 0)
            break;
        next = next->next;
    }

    if (strcmp(next->key, node->key) == 0)
    {
        free(next->data);
        next->data = strdup(node->data);
        free_node(node);
        return;
    }

    next->next = node;
}

char *get_value(Node** hash, char* key)
{
    size_t idx = hashing(key);
    Node *node = hash[idx];

    while(node)
    {
        if (strcmp(node->key, key) == 0)
        {
            return (char *)(node->data);
        }
        node = node->next;
    }
    return NULL;
}