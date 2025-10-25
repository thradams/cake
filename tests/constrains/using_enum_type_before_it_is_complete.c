// Test 4: Using enum type before it is complete
#include <stdio.h>

enum IncompleteEnum myVar; // Error: Incomplete enum type used before definition

enum IncompleteEnum {
    A, B, C
};

int main() {
    return 0;
}
