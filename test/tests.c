#include "split_test.h"
#include "data_test.h"
#include "hash_test.h"
#include "disk_logic_test.h"
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

int mock_alloc_limit = -1; // -1 значит "не ограничивать"
int alloc_counter = 0;

extern void* __real_calloc(size_t nmemb, size_t size);

void* __wrap_calloc(size_t nmemb, size_t size) {
    alloc_counter++;

    if (mock_alloc_limit >= 0 && alloc_counter > mock_alloc_limit) {
        return NULL;
    }

    if (mock_alloc_limit < 0)
    {
        alloc_counter = 0;
    }

    return __real_calloc(nmemb, size);
}

extern void* __real_malloc(size_t size);

void *__wrap_malloc(size_t size)
{
    alloc_counter++;

    if (mock_alloc_limit >= 0 && alloc_counter > mock_alloc_limit) {
        return NULL;
    }

    if (mock_alloc_limit < 0)
    {
        alloc_counter = 0;
    }

    return __real_malloc(size);
}

bool open_error = false;
bool close_error = false;
bool lseek_error = false;
bool write_error = false;
bool read_error = false;

extern int __real_open(const char *__path, int __oflag, ...);

int __wrap_open(const char *path, int oflag, ...)
{
    return open_error ? -1 : 1;
}

extern int __real_close(int __fd);

int __wrap_close(int fd)
{
    return close_error ? -1 : 0;
}

extern off_t __real_lseek(int __fd, off_t __offset, int __whence);

off_t __wrap_lseek(int fd, off_t offset, int whence)
{
    return fd == -1 || lseek_error ? -1 : 0;
}

extern ssize_t __real_write(int __fd, const void *__buf, size_t __n);

ssize_t __wrap_write(int fd, const void *buf, size_t n)
{
    return fd == -1 || write_error ? -1 : 0;
}

extern ssize_t __real_read(int __fd, void *__buf, size_t __nbytes);

ssize_t __wrap_read(int fd, void *buf, size_t nbytes)
{
    if (fd == -1 || read_error)
        return -1;
    
    memset(buf, 0, nbytes);
    return nbytes;
}

int main()
{
    if (test_split_all_functions())
    {
        printf("PASS: test_split_all_functions\n");
    }
    else
    {
        printf("FAIL: test_split_all_functions\n");
    }

    if (test_data_all_functions())
    {
        printf("PASS: test_data_all_functions\n");
    }
    else
    {
        printf("FAIL: test_data_all_functions\n");
    }

    if (test_hash_all_functions())
    {
        printf("PASS: test_hash_all_functions\n");
    }
    else
    {
        printf("FAIL: test_hash_all_functions\n");
    }

    if (test_disk_logic_all_functions())
    {
        printf("PASS: test_disk_logic_all_functions\n");
    }
    else
    {
        printf("FAIL: test_disk_logic_all_functions\n");
    }

    return 0;
}