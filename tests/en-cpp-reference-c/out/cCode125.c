//en.cppreference.com/w/c/language/operator_assignment.html
#include <stdio.h>
 
int main(void)
{
    // integers
    int i = 1, j = 2, k = 3; // initialization, not assignment
 
    i = j = k;   // values of i and j are now 3
//  (i = j) = k; // Error: lvalue required
    printf("%d %d %d\n", i, j, k);
 
    // pointers
    const char c = 'A'; // initialization; not assignment
    const char *p = &c;  // initialization; not assignment
    const char **cpp = &p; // initialization; not assignment
 
//  cpp = &p;   // Error: char** is not convertible to const char**
    *cpp = &c;  // OK, char* is convertible to const char*
    printf("%c \n", **cpp);
    cpp = 0;    // OK, null pointer constant is convertible to any pointer
 
    // arrays
    int arr1[2] = {1,2}, arr2[2] = {3, 4};
//  arr1 = arr2; // Error: cannot assign to an array
    printf("arr1[0]=%d arr1[1]=%d arr2[0]=%d arr2[1]=%d\n",
            arr1[0],   arr1[1],   arr2[0],   arr2[1]);
 
    struct { int arr[2]; } sam1 = { {5, 6} }, sam2 = { {7, 8} };
    sam1 = sam2; // OK: can assign arrays wrapped in structs
 
    printf("%d %d \n", sam1.arr[0], sam1.arr[1]);
}