//en.cppreference.com/w/c/memory/realloc.html
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
void print_storage_info(const int* next, const int* prev, int ints)
{
    if (next)
        printf("%s location: %p. Size: %d ints (%ld bytes).\n",
               (next != prev ? "New" : "Old"), (void*)next, ints, ints * sizeof(int));
    else
        printf("Allocation failed.\n");
}
 
int main(void)
{
    const int pattern[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const int pattern_size = sizeof pattern / sizeof(int);
    int *next = NULL, *prev = NULL;
 
    if ((next = (int*)malloc(pattern_size * sizeof *next))) // allocates an array
    {
        memcpy(next, pattern, sizeof pattern); // fills the array
        print_storage_info(next, prev, pattern_size);
    }
    else
        return EXIT_FAILURE;
 
    // Reallocate in cycle using the following values as a new storage size.
    const int realloc_size[] = {10, 12, 512, 32768, 65536, 32768};
 
    for (int i = 0; i != sizeof realloc_size / sizeof(int); ++i)
    {
        if ((next = (int*)realloc(prev = next, realloc_size[i] * sizeof(int))))
        {
            print_storage_info(next, prev, realloc_size[i]);
            assert(!memcmp(next, pattern, sizeof pattern));  // is pattern held
        }
        else // if realloc failed, the original pointer needs to be freed
        {
            free(prev);
            return EXIT_FAILURE;
        }
    }
 
    free(next); // finally, frees the storage
    return EXIT_SUCCESS;
}