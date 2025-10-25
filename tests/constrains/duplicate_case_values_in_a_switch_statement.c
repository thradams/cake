// Test 3: Duplicate case values in a switch statement


int main() {
    int x = 1;
    switch (x) {
        case 1:
            //printf("Case 1\n");
            break;
        case 1: // Error: Duplicate case value
            //printf("Duplicate case 1\n");
            break;
        default:
            //printf("Default\n");
    }
    return 0;
}
