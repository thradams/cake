/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct token {
    int type;
    struct token * next;
    struct token * prev;
};

void print_line(struct token * p)
{
    struct token * prev;

    prev = p->prev;
    if (prev)
    {
        struct token * next;

        next = prev;
        while (next && next->type != 0)
        {
            next = next->next;
        }
    }
}
