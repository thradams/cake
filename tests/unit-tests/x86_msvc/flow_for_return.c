/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

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
