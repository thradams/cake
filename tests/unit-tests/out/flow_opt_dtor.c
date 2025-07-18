struct X {
    struct Y * pY;
};

struct Y {
    char * text;
};


void free(void * p);
char *strdup(char * s);

void f(struct X * pX)
{
    if (pX)
    {
        free(pX->pY->text);
        pX->pY->text = strdup("a");
    }
}

