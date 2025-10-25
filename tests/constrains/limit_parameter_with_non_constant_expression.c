// Test 8: limit parameter with non-constant expression

int max_limit = 10;
#embed "resource.txt" limit(max_limit) // Error: limit parameter must be a constant expression

int main() {
    return 0;
}
