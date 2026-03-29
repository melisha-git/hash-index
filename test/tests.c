#include "split_test.h"
#include <stdio.h>
#include <stddef.h>

int mock_calloc_limit = -1; // -1 значит "не ограничивать"
int calloc_counter = 0;

extern void* __real_calloc(size_t nmemb, size_t size);

void* __wrap_calloc(size_t nmemb, size_t size) {
    calloc_counter++;

    if (mock_calloc_limit >= 0 && calloc_counter > mock_calloc_limit) {
        return NULL;
    }

    if (mock_calloc_limit < 0)
    {
        calloc_counter = 0;
    }

    return __real_calloc(nmemb, size);
}

int main()
{
    if (test_split_all_functions())
    {
        printf("PASS: test_split_all_functions\n");
    }
}