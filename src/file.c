struct X {
    int i;
};

struct Y {
    void (*PF) (struct X* s1);
};

void f(struct X* pX, struct Y* pY) {}