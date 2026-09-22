// Test 1: Non-integer type in switch controlling expression

int main() {
    float x = 3.14; //lint 74 constant expression not exactly representable
    switch (x) { //lint 940 switch controlling expression must have integer type
        case 1:
            //printf("Case 1\n");
            break;
        default:
            //printf("Default\n");
    }
    return 0;
}
