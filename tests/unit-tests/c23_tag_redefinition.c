struct X { int a; };
struct X { int a; };
struct X { long a; }; //lint 2070

union U { int a; };
union U { int a; int b; }; //lint 2070

int main() {}
