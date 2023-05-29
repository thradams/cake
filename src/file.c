
struct X {
    int a[10];

    /*uncomment the next line*/
    char * text;
};

void x_destroy(struct X* p);

int main()
{
    struct X x = {};
    x_destroy(&x);
}



void x_destroy(struct X* p)
{
    static_assert(_Hashof(struct X) == 283780300);
}


