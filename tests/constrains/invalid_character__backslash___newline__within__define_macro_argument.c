// Test 6: Invalid character (backslash + newline) within #define macro argument


#define ADD(x, \ 
            y) ((x) + (y)) // Error: Backslash and newline not allowed within macro argument list

int main() {
    ADD(2, 3);
    return 0;
}
