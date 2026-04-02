#include "disk_logic_test.h"

#include "../data.h"
#include <unistd.h>

extern bool open_error;
extern bool close_error;
extern bool lseek_error;
extern bool write_error;
extern bool read_error;
extern int mock_alloc_limit;
extern int alloc_counter;

bool test_disk_logic_all_functions()
{
    bool result = true;

    result = result && test_create_file();
    result = result && test_delete_file();
    result = result && test_write_to_file();
    result = result && test_read_file();

    return result;
}

bool test_create_file()
{
    bool result = true;
    int fd = -1;

    result = result && create_file(&fd, "");

    fd = -1;

    {
        open_error = true;
        result = result && !(create_file(&fd, ""));
        open_error = false;
    }

    return result;
}

bool test_delete_file()
{
    bool result = true;

    result = result && delete_file(1);
    result = result && !(delete_file(-1));

    {
        close_error = true;
        result = result && !(delete_file(1));
        result = result && !(delete_file(-1));
        close_error = false;
    }

    return result;
}

bool test_write_to_file()
{
    bool result = true;
    struct Data data;

    result = result && (write_to_file(-1, data) == -1);
    result = result && (write_to_file(1, data) == 0);

    {
        lseek_error = true;
        result = result && (write_to_file(-1, data) == -1);
        result = result && (write_to_file(1, data) == -1);
        lseek_error = false;
    }

    {
        write_error = true;
        result = result && (write_to_file(-1, data) == -1);
        result = result && (write_to_file(1, data) == -1);
        write_error = false;
    }

    return result;
}

bool test_read_file()
{
    bool result = true;
    struct Data data;

    data = read_file(1, 0);
    result = result && (data.key != NULL);
    result = result && (data.value != NULL);
    destroy_data(&data);

    data = read_file(-1, 0);
    result = result && (data.key == NULL);
    result = result && (data.value == NULL);

    {
        lseek_error = true;
        data = read_file(1, 0);
        result = result && (data.key == NULL);
        result = result && (data.value == NULL);

        data = read_file(-1, 0);
        result = result && (data.key == NULL);
        result = result && (data.value == NULL);
        lseek_error = false;
    }

    {
        read_error = true;
        data = read_file(1, 0);
        result = result && (data.key == NULL);
        result = result && (data.value == NULL);

        data = read_file(-1, 0);
        result = result && (data.key == NULL);
        result = result && (data.value == NULL);
        read_error = false;
    }

    {
        mock_alloc_limit = 0;
        alloc_counter = 0;
        data = read_file(1, 0);
        result = result && (data.key == NULL);
        result = result && (data.value == NULL);

        data = read_file(-1, 0);
        result = result && (data.key == NULL);
        result = result && (data.value == NULL);
        mock_alloc_limit = -1;
        alloc_counter = 0;
    }

    {
        mock_alloc_limit = 1;
        alloc_counter = 0;
        data = read_file(1, 0);
        result = result && (data.key == NULL);
        result = result && (data.value == NULL);

        data = read_file(-1, 0);
        result = result && (data.key == NULL);
        result = result && (data.value == NULL);
        mock_alloc_limit = -1;
        alloc_counter = 0;
    }

    return result;
}