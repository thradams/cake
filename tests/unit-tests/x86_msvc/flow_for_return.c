/* Cake 0.12.26 x86_msvc */
struct X {
    int i;
};


void fill_options(struct X * options)
{
    {
        int i;

        i = 1;
        for (; i < 2; i++)
        {
            options->i = 1;
            return;
        }
    }
}


