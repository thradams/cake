// Test 2: thread_local without static or extern in block scope

int main() {
    thread_local int y = 10; //lint 980 thread_local without static or extern in block scope
    return 0;
}
