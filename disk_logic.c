#include "disk_logic.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

bool create_file(int *fd, char* path)
{
    delete_file(*fd);
    (*fd) = open(path, O_RDWR | O_CREAT | O_APPEND, 0666);
    
    if ((*fd) == -1)
    {
        perror("open");
        return false;
    }

    return true;
}

void delete_file(int fd)
{
    if (fd != -1)
    {
        close(fd);
    }
}

off_t write_to_file(int fd, struct Data data)
{
    off_t offset = -1;

    if (fd == -1)
    {
        return -1;
    }

    offset = lseek(fd, 0, SEEK_END);

    if (write(fd, &data.data_size, sizeof(size_t)) == -1)
        return -1;
    
    if (write(fd, data.data, data.data_size) == -1)
        return -1;
    
    if (write(fd, "\n", 1) == -1)
        return -1;

    return offset;
}

struct Data read_file(int fd, off_t offset)
{
    struct Data result;
    result.data = NULL;

    if (fd == -1)
    {
        return result;
    }

    lseek(fd, offset, SEEK_SET);
    read(fd, &result.data_size, sizeof(size_t));
    result.data = calloc(result.data_size, sizeof(char));
    read(fd, result.data, result.data_size);
    return result;
}