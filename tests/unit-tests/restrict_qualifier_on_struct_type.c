// Test 6: restrict qualifier on struct type

struct Point {
    int x, y;
};

struct Point restrict p; //lint 2230

int main() {
    return 0;
}
