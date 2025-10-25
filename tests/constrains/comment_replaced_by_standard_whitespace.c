// Test 7: Comment replaced by standard whitespace


#define VALUE /* This comment should be replaced by space */ 42

int main() {
    printf("%d\n", VALUE); // Expected output: 42
    return 0;
}
