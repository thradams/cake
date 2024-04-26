#pragma safety enable



void free(void */*_Owner*/ /*_Opt*/ ptr);
void */*_Owner*/ /*_Opt*/ malloc(int size);
char */*_Owner*/ /*_Opt*/ strdup(const char *);

struct X
{
    char */*_Owner*/ /*_Opt*/ name;
};

struct Y
{
    struct X x;
    struct X * /*_Opt*/ px;
};

void x_destroy(struct X */*_Obj_owner*/ /*_Opt*/ p)
{
    if (p)
     free(p->name);
}

void f(struct Y *p)
{
    x_destroy(p->px);
#pragma cake diagnostic check "-Wmust-use-address-of"
}

int main()
{
    struct Y y = {0};
    struct X *p = &y.x;
    x_destroy(&y.x);
}
