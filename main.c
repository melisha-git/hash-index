#include <stdio.h>
#include <stdlib.h>
#include "split.h"
#include "hash.h"
#include "disk_logic.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

bool directory_exists(char *path)
{
    struct stat stats;

    return stat(path, &stats) == 0 && S_ISDIR(stats.st_mode);
}

bool file_exists(char *path)
{
    struct stat stats;

    return stat(path, &stats) == 0;
}

bool file_is_full(char *path)
{
    struct stat stats;

    if (stat(path, &stats) == 0)
    {
        if (stats.st_size >= 4096)
        {
            return true;
        }
    }

    return false;
}

char* get_next_name(int meta_fd)
{
    time_t t;
    struct stat stats;
    struct Data data;
    char *result;

    off_t offset = lseek(meta_fd, 0, SEEK_SET);

    while ((data = read_file(meta_fd, offset)).key != NULL)
    {
        if (!file_is_full(data.key))
        {
            result = strdup(data.key);
            destroy_data(&data);
            return result;
        }
        destroy_data(&data);
        offset = lseek(meta_fd, 0, SEEK_CUR);
    }

    time(&t);
    result = strdup(ctime(&t));
    result[strlen(result) - 1] = '\0';

    return result;
}

bool read_data(Node **hash, int fd)
{
    off_t offset;
    struct Data data;

    if (fd == -1 || hash == NULL)
    {
        return false;
    }

    offset = lseek(fd, 0, SEEK_SET);

    while ((data = read_file(fd, offset)).key != NULL)
    {
        char buf[20];
        sprintf(buf, "%ld", offset);
        set_value(hash, data.key, buf);
        offset = lseek(fd, 0, SEEK_CUR);
        destroy_data(&data);
    }
}

bool read_metadata(Node **hash, int meta_fd)
{
    struct Data data;
    off_t offset;
    int fd = -1;

    if (meta_fd == -1 || hash == NULL)
    {
        return false;
    }

    offset = lseek(meta_fd, 0, SEEK_SET);

    while ((data = read_file(meta_fd, offset)).key != NULL)
    {
        char path[256] = {0};
        sprintf(path, "data/%s", (char *)data.key);

        offset = lseek(fd, 0, SEEK_CUR);

        if (!file_exists(path))
        {
            destroy_data(&data);
            continue;
        }

        if (!create_file(&fd, path))
        {
            destroy_data(&data);
            continue;
        }

        read_data(hash, fd);

        destroy_data(&data);
        delete_file(fd);
    }

    return true;
}

int handle_command(size_t argc, char** argv, Node** hash, int fd)
{
    char command;
    char *key;
    char* value;

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
        off_t offset = write_to_file(fd, data);
        destroy_data(&data);

        char buf[20];
        sprintf(buf, "%ld", offset);
        set_value(hash, key, buf);
        break;
    }
    case 'g':
    {
        char* offset_str = get_value(hash, key);

        if (offset_str == NULL)
            break;

        off_t offset = atoll(offset_str);
        struct Data data = read_file(fd, offset);
        printf("value = %s\n", (char *)data.value);
        destroy_data(&data);
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
    int meta_fd = -1;
    int fd = -1;
    char path[256] = {0};
    struct Data metadata;

    if (!directory_exists("data") && mkdir("data", 0777) != 0)
    {
        printf("ERROR: Can not create directory 'data'\n");
        return;
    }

    if (!create_file(&meta_fd, "data/metadata"))
    {
        printf("ERROR: Can not open file 'metadata'");
        return;
    }

    read_metadata(hash, meta_fd);

    while (scanf(" %[^\n]s", line) != EOF)
    {
        char** argv = split(line, ' ');
        size_t argc = 0;

        while(argv && argv[argc])
            argc++;

        if (fd == -1)
        {
            char *filename = get_next_name(meta_fd);
            sprintf(path, "data/%s", filename);

            if (!create_file(&fd, path))
            {
                printf("ERROR: Can not open file");
                perror("error");
                return;
            }

            if (!file_exists(path))
            {
                metadata = make_data(filename, NULL);
                write_to_file(meta_fd, metadata);
                destroy_data(&metadata);
            }
            free(filename);
        }

        handle_command(argc, argv, hash, fd);
        free_splits(argv);

        if (file_is_full(path))
        {
            delete_file(fd);
            fd = -1;
        }
    }

    delete_file(meta_fd);
}

int main()
{
    Node** hash = init_hash();
    read_commands(hash);
    destroy_hash(hash);
    return 0;
}