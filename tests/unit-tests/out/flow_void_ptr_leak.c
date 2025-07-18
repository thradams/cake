struct X {
    char * text;
};


void x_destroy(struct X * p);

void x_delete(struct X * p)
{
    if (p)
    {
        x_destroy(p);
    }
}

