#pragma safety enable


void* /*_Owner*/ malloc(unsigned long size);
void free(void* /*_Owner*/ /*_Opt*/ ptr);

struct X {
    char* /*_Owner*/ /*_Opt*/ text;
};

void f(struct X* /*_Owner*/ p1, struct X* /*_Out*/ /*_Owner*/* p2)
{
    *p2 = p1;
}

int main()
{
    struct X* /*_Owner*/ p1 = malloc(sizeof * p1);
    if (p1)
    {
        p1->text = 0;
        struct X* /*_Owner*/ /*_Opt*/ p2 = 0;
        f(p1, &p2);

        free(p2->text);
        free(p2);
    }
}
