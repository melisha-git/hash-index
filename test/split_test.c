#define INCLUDE_UTIL
#include "../split.h"
#include <string.h>
#include <stdio.h>
#include "split_test.h"
#include <stdlib.h>

extern int mock_calloc_limit;
extern int calloc_counter;

bool check_words(char **a, const char **b)
{
    if (!a && !b)
    {
        return true;
    }

    for (;a && (*a) && b && (*b); a++, b++)
    {
        if (strcmp(*a, *b) != 0)
        {
            printf("ERROR: %s != %s\n", *a, *b);
            return false;
        }
    }

    if ((a && !b) || (b && !a) || ((*a && !(*b)) || (!*a && *b)))
    {
        printf("ERROR: len(a) != len(b)\n");
        return false;
    }

    return true;
}

bool check_equals(size_t a, size_t b)
{
    if (a != b)
    {
        printf("ERROR: %lu != %lu\n", a, b);
        return false;
    }

    return true;
}

bool test_split_all_functions()
{
    bool result = true;

    result = result && test_words_count();
    result = result && test_split();
    result = result && test_free_split();

    return result;
}

bool test_words_count()
{
    bool result = true;

    result = result && check_equals(str_words_count("123 456 789", ' '), 3);
    result = result && check_equals(str_words_count("123  456  789    q", ' '), 4);
    result = result && check_equals(str_words_count("123            456             789   q         1", ' '), 5);
    result = result && check_equals(str_words_count("       123 456        ", ' '), 2);

    return result;
}

bool test_split()
{
    bool result = true;
    char **data;

    {
        mock_calloc_limit = 2;
        calloc_counter = 0;
        data = split("123  456  789    q", ' ');
        result = result && check_words(data, NULL);

        mock_calloc_limit = 3;
        calloc_counter = 0;
        data = split("123 456 789", ' ');
        result = result && check_words(data, NULL);

        mock_calloc_limit = 0;
        calloc_counter = 0;
        data = split("123  456  789    q", ' ');
        result = result && check_words(data, NULL);

        mock_calloc_limit = 6;
        calloc_counter = 0;
        data = split("123            456             789   q         1", ' ');
        result = result && check_words(data, (const char *[]){"123", "456", "789", "q", "1", NULL});
        free_splits(data);

        mock_calloc_limit = -1;
    }

    data = split("123 456 789", ' ');
    result = result && check_words(data, (const char *[]){"123", "456", "789", NULL});
    free_splits(data);

    data = split("123  456  789    q", ' ');
    result = result && check_words(data, (const char *[]){"123", "456", "789", "q", NULL});
    free_splits(data);

    data = split("123            456             789   q         1", ' ');
    result = result && check_words(data, (const char *[]){"123", "456", "789", "q", "1", NULL});
    free_splits(data);

    data = split("       123 456        ", ' ');
    result = result && check_words(data, (const char *[]){"123", "456", NULL});
    free_splits(data);

    return result;
}

bool test_free_split()
{
    char** data = split("123;456", ';');
    free_splits(data);
    free_splits(NULL);

    return true;
}