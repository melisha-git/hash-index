#include <stdio.h>
#include "split.h"
#include "hash.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

void deinit_file(int fd)
{
    if (fd != -1)
    {
        close(fd);
    }
}

bool init_file(int *fd)
{
    deinit_file(*fd);
    (*fd) = open("file", O_RDWR | O_CREAT | O_APPEND, 0666);
    
    if ((*fd) == -1)
    {
        perror("open");
        return false;
    }

    return true;
}

off_t write_to_file(int fd, void* data, size_t data_size)
{
    off_t offset = -1;

    if (fd == -1)
    {
        return -1;
    }

    offset = lseek(fd, 0, SEEK_END);

    if (write(fd, &data_size, sizeof(size_t)) == -1)
        return -1;
    
    if (write(fd, data, data_size) == -1)
        return -1;
    
    if (write(fd, "\n", 1) == -1)
        return -1;

    return offset;
}

struct Data
{
    void *data;
    size_t data_size;
};

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

struct Data make_data(char *key, char *value)
{
    struct Data data;

    char numbers_buffer[20];
    size_t key_size = strlen(key);
    size_t value_size = strlen(value);

    data.data_size = key_size + value_size + 2;

    data.data = calloc(data.data_size, sizeof(char));
    memcpy(data.data, key, key_size);
    memcpy(data.data + key_size, ";", 1);
    memcpy(data.data + key_size + 1, value, value_size);
    memcpy(data.data + key_size + value_size + 1, ";", 1);
    printf("data = %s\n", (char *)data.data);

    return data;
}

int handle_command(size_t argc, char** argv, Node** hash)
{
    char command;
    char *key;
    char* value;
    int fd = -1;

    if (!init_file(&fd))
    {
        printf("ERROR: Can not open file");
        return -1;
    }

    if (argc < 2)
    {
        printf("ERROR: Too few arguments '%ld'\n", argc);
        return -1;
    }

    if (strcmp(argv[0], "s") != 0 && strcmp(argv[0], "g") != 0)
    {
        printf("ERROR: Command %s not found\n", argv[0]);
        return -1;
    }

    command = argv[0][0];
    key = argv[1];

    switch (command)
    {
    case 's':
    {
        size_t data_size;
        if (argc < 3)
        {
            printf("ERROR: Too few arguments '%ld'\n", argc);
            free_splits(argv);
            return -1;
        }
        value = argv[2];

        struct Data data = make_data(key, value);
        off_t offset = write_to_file(fd, data.data, data.data_size);
        free(data.data);

        char buf[20];
        sprintf(buf, "%ld", offset);
        set_value(hash, key, buf);
        break;
    }
    case 'g':
    {
        off_t offset = atoll(get_value(hash, key));
        struct Data data = read_file(fd, offset);
        break;
    }
    default:
        printf("ERROR: Command not found");
        break;
    }

    return 1;
}

void read_commands(Node** hash)
{
    char line[1024];

    while (scanf(" %[^\n]s", line) != EOF)
    {
        char** argv = split(line, ' ');
        size_t argc = 0;

        while(argv && argv[argc])
            argc++;

        handle_command(argc, argv, hash);
        free_splits(argv);
    }
}

int main()
{
    Node** hash = init_hash();
    read_commands(hash);
    free_hash(hash);
    return 0;
}