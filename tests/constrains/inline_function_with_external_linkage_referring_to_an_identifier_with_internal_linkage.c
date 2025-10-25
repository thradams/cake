// Test 3: Inline function with external linkage referring to an identifier with internal linkage


static int internal_var = 10;

inline void my_function() {
    internal_var; // Error: Inline function with external linkage cannot refer to internal linkage identifier
}
