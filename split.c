#include "split.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

size_t str_words_count(char *str, char delimether)
{
    size_t result = 0;
    size_t pos = 0;
    size_t i = 0;

    for (; i < strlen(str); ++i)
    {
        if (str[i] == delimether)
        {
            if (i != 0 && i > pos)
            {
                result++;
            }
            pos = i + 1;
        }
    }

    return result + (i > pos);
}

void free_splits(char **split_str)
{
    char **copy = split_str;
    while (split_str && (*split_str))
    {
        free((*split_str));
        split_str++;
    }
    
    free(copy);

    split_str = NULL;
}

char **split(char *str, char delimether)
{
    char **result;
    size_t pos = 0;
    size_t words = str_words_count(str, delimether);
    size_t word_idx = 0;
    size_t i = 0;

    result = (char **)calloc(words + 1, sizeof(char *));

    if (!result)
        return NULL;

    for (; i < strlen(str); ++i)
    {
        if (str[i] == delimether)
        {
            if (i != 0 && i > pos)
            {
                result[word_idx] = (char *)calloc(i - pos + 1, sizeof(char));

                if (!result[word_idx])
                {
                    free_splits(result);
                    return NULL;
                }

                strncpy(result[word_idx], &str[pos], i - pos);
                word_idx++;
            }
            pos = i + 1;
        }
    }

    if (i > pos)
    {
        result[word_idx] = (char *)calloc(i - pos + 1, sizeof(char));

        if (!result[word_idx])
        {
            free_splits(result);
            return NULL;
        }

        strncpy(result[word_idx], &str[pos], i - pos);
        word_idx++;
    }

    return result;
}