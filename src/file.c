union Foo {
    char a;
    char b;
};

static_assert(sizeof(union Foo) == 1);  // fails
int main(){}
