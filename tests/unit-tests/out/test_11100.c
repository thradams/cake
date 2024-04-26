#pragma safety enable


void* /*_Owner*/ /*_Opt*/ malloc(unsigned long size);

struct X {
    char* /*_Owner*/ text;
};

void x_delete(struct X* /*_Owner*/  /*_Opt*/ p);

int main()
{
    struct X* /*_Owner*/ /*_Opt*/ p = malloc(sizeof(struct X));
    x_delete(p);
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
}

