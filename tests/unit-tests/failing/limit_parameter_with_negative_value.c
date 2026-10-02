// Test 4: limit parameter with negative value

#embed "resource.txt" limit(-5) // Error: limit parameter must not evaluate to a negative value

int main() {
    return 0;
}
