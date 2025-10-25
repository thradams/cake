// Test 4: has_embed expression with unsupported parameter


#if __has_embed(<resource>, unsupported_parameter) // Error: Unsupported embed parameter
    #define EMBED_FOUND 1
#endif

int main() {
    return 0;
}
