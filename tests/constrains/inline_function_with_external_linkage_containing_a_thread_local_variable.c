// Test 6: Inline function with external linkage containing a thread-local variable


inline void my_function() {
    thread_local int counter = 0; // Error: Inline function with external linkage cannot have thread-local variable
    counter++;
}
