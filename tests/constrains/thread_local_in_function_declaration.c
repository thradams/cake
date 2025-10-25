// Test 4: thread_local in function declaration
#include <stdio.h>

thread_local void my_function() { // Error: thread_local cannot appear in function declarations
}
