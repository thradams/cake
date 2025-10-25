// Test 7: Enum type without enumerator list before completion


enum ForwardEnum; // Error: Enum declaration without enumerator list before completion

int main() {
    enum ForwardEnum var = A;
    return 0;
}
