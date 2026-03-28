#include <stdio.h>
#include <stdlib.h>
#include "split.h"
#include "hash.h"
#include "disk_logic.h"

int handle_command(size_t argc, char** argv, Node** hash)
{
    char command;
    char *key;
    char* value;
    int fd = -1;

    if (!create_file(&fd, "file"))
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
        off_t offset = write_to_file(fd, data);
        destroy_data(data);

        char buf[20];
        sprintf(buf, "%ld", offset);
        set_value(hash, key, buf);
        break;
    }
    case 'g':
    {
        off_t offset = atoll(get_value(hash, key));
        struct Data data = read_file(fd, offset);
        destroy_data(data);
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