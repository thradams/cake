// Test 5: Invalid nullptr_t conversion


int main() {
    int x = (int)nullptr; //lint 1810 cannot cast nullptr_t to this type
    return 0;
}
