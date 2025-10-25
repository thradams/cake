// Test 1: Non-integer type in switch controlling expression

int main() {
    float x = 3.14;
    switch (x) { // Error: switch controlling expression must be of integer type
        case 1:
            //printf("Case 1\n");
            break;
        default:
            //printf("Default\n");
    }
    return 0;
}
