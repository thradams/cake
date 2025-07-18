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

