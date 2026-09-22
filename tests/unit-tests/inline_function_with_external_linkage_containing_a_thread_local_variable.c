// Test 6: Inline function with external linkage containing a thread-local variable


inline void my_function() {
    thread_local int counter = 0; //lint 980 thread_local without static or extern in block scope
    counter++;
}
