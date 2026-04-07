#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Data make_data(const char *key, const char *value)
{
    struct Data data = {0, 0, NULL, NULL};

    if (key == NULL)
    {
        return data;
    }

    data.key_size = strlen(key);
    data.value_size = value == NULL ? 0 : strlen(value);

    data.key = calloc(data.key_size + 1, sizeof(char));
    data.value = calloc(data.value_size + 1, sizeof(char));
    
    if (!data.key || !data.value)
    {
        destroy_data(&data);
        return data;
    }

    memcpy(data.key, key, data.key_size);
    memcpy(data.value, value, data.value_size);

    return data;
}

void destroy_data(struct Data *data)
{
    if (!data)
        return;

    if (data->key)
        free(data->key);

    if (data->value)
        free(data->value);

    data->key = NULL;
    data->value = NULL;
    data->key_size = 0;
    data->value_size = 0;
}