//en.cppreference.com/w/c/experimental/dynamic/getline.html
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif
 
#include <stdio.h>
#include <stdlib.h>
void get_y_or_n(void)
{
    char *response = NULL;
    size_t len;
    printf("Continue? [y] n: ");
    if((getline(&response, &len, stdin) < 0) || (len && response[0] == 'n')) {
        free(response);
        exit(0);
    }
    free(response);
    return;
}
int main(void) 
{
    get_y_or_n();
}