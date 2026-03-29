#include "data_test.h"
#include "../data.h"
#include <string.h>

extern int mock_calloc_limit;
extern int calloc_counter;

bool cmpdata(struct Data a, struct Data b)
{
    return a.key_size == b.key_size &&
           a.value_size == b.value_size &&
           ((a.key == NULL && b.key == NULL) || strcmp(a.key, b.key) == 0) &&
           ((a.value == NULL && b.value == NULL) || strcmp(a.value, b.value) == 0);
}

bool test_data_all_functions()
{
    bool result = true;

    result = result && test_make_data();
    result = result && test_destroy_data();

    return result;
}

bool test_make_data()
{
    bool result = true;
    struct Data data;

    {
        mock_calloc_limit = 0;
        calloc_counter = 0;
        data = make_data("qwerty", "zxc01");
        result = result && cmpdata(data, (struct Data){0, 0, NULL, NULL});

        mock_calloc_limit = 1;
        calloc_counter = 0;
        data = make_data("qwerty", "zxc01");
        result = result && cmpdata(data, (struct Data){0, 0, NULL, NULL});

        mock_calloc_limit = 2;
        calloc_counter = 0;
        data = make_data("qwerty", "zxc01");
        result = result && cmpdata(data, (struct Data){6, 5, "qwerty", "zxc01"});
        destroy_data(&data);

        mock_calloc_limit = -1;
    }

    data = make_data("12", "12345");
    result = result && cmpdata(data, (struct Data){2, 5, "12", "12345"});
    destroy_data(&data);

    data = make_data("", "12345678");
    result = result && cmpdata(data, (struct Data){0, 8, "", "12345678"});
    destroy_data(&data);

    data = make_data("123", "");
    result = result && cmpdata(data, (struct Data){3, 0, "123", ""});
    destroy_data(&data);

    return result;
}

bool test_destroy_data()
{
    bool result = true;
    struct Data data;

    data = make_data("12", "12345");
    destroy_data(&data);
    result = result && cmpdata(data, (struct Data){0, 0, NULL, NULL});

    data = make_data("", "12345678");
    destroy_data(&data);
    result = result && cmpdata(data, (struct Data){0, 0, NULL, NULL});

    data = make_data("123", "");
    destroy_data(&data);
    result = result && cmpdata(data, (struct Data){0, 0, NULL, NULL});

    return result;
}