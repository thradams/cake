#pragma safety enable

struct X {
    int i;
};

void fill_options(struct X* options)
{
    for (int i = 1; i < 2; i++)
    {
        options->i = 1; //cannot say options end lifetime (caused by return)
        return;
    }    
}