// Test 3: Incompatible structure types

struct PointA {
    int x, y;
};

struct PointB {
    int x, y;
};

int main() {
    int cond = 1;
    struct PointA a = {1, 2};
    struct PointB b = {3, 4};
    struct PointA result = cond ? a : b; //lint 950 950 650 incompatible types
    return 0;
}
