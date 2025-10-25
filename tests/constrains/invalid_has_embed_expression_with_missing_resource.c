// Test 9: Invalid has_embed expression with missing resource

#if __has_embed() // Error: Missing resource in has_embed expression
    #define EMBED_RESOURCE_FOUND 1
#endif

int main() {
    return 0;
}
