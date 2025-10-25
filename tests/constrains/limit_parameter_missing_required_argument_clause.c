// Test 2: limit parameter missing required argument clause

#embed "resource.txt" limit // Error: limit parameter requires an argument clause in the form (constant-expression)

int main() {
    return 0;
}
