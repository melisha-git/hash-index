#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Data make_data(char *key, char *value)
{
    struct Data data;

    size_t key_size = strlen(key);
    size_t value_size = strlen(value);

    data.data_size = key_size + value_size;

    data.data = calloc(data.data_size + 1, sizeof(char));
    memcpy(data.data, key, key_size);
    memcpy(data.data + key_size, ";", 1);
    memcpy(data.data + key_size + 1, value, value_size);
    memcpy(data.data + key_size + value_size + 1, ";", 1);
    printf("data = %s\n", (char *)data.data);

    return data;
}

void destroy_data(struct Data data)
{
    free(data.data);
    data.data_size = 0;
}