struct X {
    char * p;
};


char *strdup(char * s);

int main()
{
    struct X  x;

    x.p = strdup("a");
}

