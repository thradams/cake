#pragma safety enable


void free(void* _Owner _Opt p);

struct token
{
    int type;
    struct token* next;
    struct token* prev;
};

void print_line(struct token* p)
{
    struct token* prev = p->prev;
    if (prev)
    {
        struct token* next = prev;
        while (next && next->type != 0)
        {
            next = next->next;
        }
    }
}
