//en.cppreference.com/w/c/language/initialization.html
#include <stdlib.h>
int a[2]; // initializes a to {0, 0}
int main(void)
{
    int i;          // initializes i to an indeterminate value
    static int j;   // initializes j to 0
    int k = 1;      // initializes k to 1
 
    // initializes int x[3] to 1,3,5
    // initializes int* p to &x[0]
    int x[] = { 1, 3, 5 }, *p = x;
 
    // initializes w (an array of two structs) to
    // { { {1,0,0}, 0}, { {2,0,0}, 0} }
    struct {int a[3], b;} w[] = {[0].a = {1}, [1].a[0] = 2};
 
    // function call expression can be used for a local variable
    char* ptr = malloc(10);
    free(ptr);
 
//  Error: objects with static storage duration require constant initializers
//  static char* ptr = malloc(10);
 
//  Error: VLA cannot be initialized
//  int vla[n] = {0};
}