// Test 7: limit parameter with invalid expression format

#embed "resource.txt" limit(10 +) // Error: Invalid expression format for limit parameter

int main() {
    return 0;
}
