#ifndef _DATA_H_
#define _DATA_H_

#include <sys/types.h>

struct Data
{
    size_t key_size;
    size_t value_size;
    void *key;
    void *value;
};

struct Data make_data(const char *key, const char *value);
void destroy_data(struct Data *data);

#endif