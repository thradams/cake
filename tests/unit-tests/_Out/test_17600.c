#pragma safety enable


void free(void* /*_Owner*/ /*_Opt*/ p);

struct token
{
    int type;
    struct token* /*_Opt*/ next;
    struct token* /*_Opt*/ prev;
};

void print_line(struct token* p)
{
    struct token* /*_Opt*/ prev = p->prev;
    if (prev)
    {
        struct token* /*_Opt*/ next = prev;
        while (next && next->type != 0)
        {
            next = next->next;
        }
    }
}

