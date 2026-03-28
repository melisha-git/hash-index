#ifndef _DATA_H_
#define _DATA_H_

#include <sys/types.h>

struct Data
{
    void *data;
    size_t data_size;
};

struct Data make_data(char *key, char *value);
void destroy_data(struct Data data);

#endif