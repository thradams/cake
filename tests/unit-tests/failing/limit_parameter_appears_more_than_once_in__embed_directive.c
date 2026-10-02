// Test 1: limit parameter appears more than once in #embed directive

#embed "resource.txt" limit(10) limit(20) // Error: limit parameter can appear at most once

int main() {
    return 0;
}
