// Test 3: Missing whitespace between identifier and replacement list in object-like macro

#define VALUE42 // Error: Missing whitespace between identifier and replacement list

int main() {
    VALUE;
    return 0;
}
