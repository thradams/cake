// Test 8: Duplicate cases in switch statement with conditional inclusion

#define TEST_MODE 1

int main() {
#if TEST_MODE
    int value = 1;
    switch (value) {
        case 1:
            //printf("Case 1\n");
            break;
        case 1: // Error: Duplicate case value
            //printf("Duplicate Case 1\n");
            break;
        default:
            //printf("Default\n");
    }
#endif
    return 0;
}
