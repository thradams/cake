// Test 4: Multiple default labels in a switch statement


int main() {
    int x = 2;
    switch (x) {
        case 1:
            //printf("Case 1\n");
            break;
        default:
            //printf("Default 1\n");
            break;
        default: //lint 1780 multiple default labels in one switch
            //printf("Default 2\n");
            break;
    }
    return 0;
}
