// Test 2: thread_local without static or extern in block scope

int main() {
    thread_local int y = 10; // Error: thread_local without static or extern in block scope
    return 0;
}
