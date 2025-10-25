// Test 6: limit parameter with missing parentheses

#embed "resource.txt" limit 10 // Error: limit parameter requires parentheses around constant expression

int main() {
    return 0;
}
