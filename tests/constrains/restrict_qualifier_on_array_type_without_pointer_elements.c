// Test 2: restrict qualifier on array type without pointer elements

int restrict arr[5]; // Error: restrict qualifier cannot be applied to an array without pointer elements

int main() {
    return 0;
}
