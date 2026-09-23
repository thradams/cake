struct X {
    unsigned char a : 3;
    unsigned int b : 9;
};

#ifdef _MSC_VER
static_assert(sizeof(struct X) == 8);
static_assert(alignof(struct X) == 4);
#else
static_assert(sizeof(struct X) == 4);
static_assert(alignof(struct X) == 4);
#endif

int main() {}
