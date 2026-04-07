#include "disk_logic.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

bool create_file(int *fd, char* path)
{
    delete_file(*fd);
    (*fd) = open(path, O_RDWR | O_CREAT | O_APPEND, 0666);

    return (*fd) != -1;
}

bool delete_file(int fd)
{
    if (fd != -1)
    {
        return close(fd) == 0;
    }

    return false;
}

off_t write_to_file(int fd, struct Data data)
{
    off_t offset = -1;

    if (fd == -1)
    {
        return -1;
    }

    offset = lseek(fd, 0, SEEK_END);

    if (write(fd, &data.key_size, sizeof(size_t)) == -1 ||
        write(fd, data.key, data.key_size) == -1)
        return -1;

    if (write(fd, &data.value_size, sizeof(size_t)) == -1 ||
        write(fd, data.value, data.value_size) == -1)
        return -1;

    return offset;
}

struct Data read_file(int fd, off_t offset)
{
    struct Data result;
    size_t bytes;
    result.key_size = 0;
    result.value_size = 0;
    result.key = NULL;
    result.value = NULL;

    if (fd == -1)
    {
        goto cleanup;
    }

    if (lseek(fd, offset, SEEK_SET) == -1)
    {
        goto cleanup;
    }

    if (read(fd, &result.key_size, sizeof(size_t)) != sizeof(size_t))
    {
        goto cleanup;
    }

    result.key = calloc(result.key_size + 1, sizeof(char));

    if (result.key == NULL)
    {
        goto cleanup;
    }

    if (read(fd, result.key, result.key_size) == -1)
    {
        goto cleanup;
    }

    if (read(fd, &result.value_size, sizeof(size_t)) != sizeof(size_t))
    {
        goto cleanup;
    }

    result.value = calloc(result.value_size + 1, sizeof(char));

    if (result.value == NULL)
    {
        goto cleanup;
    }

    if (read(fd,result.value, result.value_size) == -1)
    {
        goto cleanup;
    }

    return result;
cleanup:
    destroy_data(&result);
    return result;
}