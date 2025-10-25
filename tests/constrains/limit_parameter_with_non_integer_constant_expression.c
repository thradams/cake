// Test 3: limit parameter with non-integer constant expression

#embed "resource.txt" limit(3.14) // Error: limit parameter requires an integer constant expression

int main() {
    return 0;
}
