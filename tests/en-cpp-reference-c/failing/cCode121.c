//en.cppreference.com/w/c/language/operator_arithmetic.html
// work in progress
int n = 4, m = 3;
int a[n][m];     // VLA of 4 VLAs of 3 ints each
int (*p)[m] = a; // p == &a[0] 
p = p + 1;       // p == &a[1] (pointer arithmetic works with VLAs just the same)
(*p)[2] = 99;    // changes a[1][2]