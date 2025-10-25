// Test 6: Non-constant expression in case label within enclosed switch


int main() {
    int outer = 1, inner_val = 2;
    switch (outer) {
        case 1:
            switch (inner_val) {
                case outer: // Error: case label must be a constant expression
                    //printf("Nested case\n");
                    break;
            }
            break;
    }
    return 0;
}
