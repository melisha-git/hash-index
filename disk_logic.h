#ifndef _DISK_LOGIC_H_
#define _DISK_LOGIC_H_

#include "data.h"
#include <stdbool.h>
#include <sys/types.h>

bool create_file(int *fd, char* path);
bool delete_file(int fd);
off_t write_to_file(int fd, struct Data data);
struct Data read_file(int fd, off_t offset);

#endif