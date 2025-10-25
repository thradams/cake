// Test 2: Inline function with external linkage containing a static variable


inline void my_function() {
    static int counter = 0; // Error: Inline function with external linkage cannot have static variable
    counter++;
}
