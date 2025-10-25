// Test 7: Correct handling of duplicate case values in nested switch statements


int main() {
    int outer = 1, inner = 1;
    switch (outer) {
        case 1:
            //printf("Outer case 1\n");
            switch (inner) {
                case 1: // This is allowed because it’s in a nested switch
                    //printf("Inner case 1\n");
                    break;
            }
            break;
    }
    return 0;
}
