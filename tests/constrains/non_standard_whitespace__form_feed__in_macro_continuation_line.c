// Test 8: Non-standard whitespace (form feed) in macro continuation line

#define MAX(a, b) ( \
    ((a) > (b)) ? (a) : (b) ) \f // Error: Form feed in macro continuation line not allowed

int main() {
    MAX(10, 20);
    return 0;
}
