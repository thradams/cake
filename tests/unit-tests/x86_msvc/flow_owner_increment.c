/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


struct X *make();
void del(struct X * p);

int main()
{
    struct X * p;

    p = make();
    p++;
    p--;
    del(p);
}


