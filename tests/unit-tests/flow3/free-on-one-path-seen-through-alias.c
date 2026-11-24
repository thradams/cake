#pragma safety enable

void* _Owner malloc(unsigned long n);
void free(void* _Owner _Nullable p);

int freed_in_while_with_break(int n)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p)
    {
        return 0;
    }
    int* q = p;
    while (n)
    {
        free(p);
        p = 0;
        break;
    }
    int r = *q; //lint 31
    free(p);
    return r;
}

int freed_in_if(int n)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p)
    {
        return 0;
    }
    int* q = p;
    if (n)
    {
        free(p);
        p = 0;
    }
    int r = *q; //lint 31
    free(p);
    return r;
}

int freed_in_one_of_two_arms(int n)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p)
    {
        return 0;
    }
    int* q = p;
    if (n > 2)
    {
        free(p);
        p = 0;
    }
    else if (n > 1)
    {
        *q = 1;
    }
    else
    {
        free(p);
        p = 0;
    }
    int r = *q; //lint 31
    free(p);
    return r;
}

int freed_arm_returns(int n)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p)
    {
        return 0;
    }
    int* q = p;
    if (n)
    {
        free(p);
        return 0;
    }
    int r = *q; /* not freed on this path */
    free(p);
    return r;
}

int used_in_the_other_arm(int n)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p)
    {
        return 0;
    }
    int* q = p;
    int r = 0;
    if (n)
    {
        free(p);
        p = 0;
    }
    else
    {
        r = *q; /* not freed on this path */
    }
    free(p);
    return r;
}

struct table
{
    int* _Owner items;
    int size;
    int capacity;
};

int pointer_replaced_after_the_arm(struct table* t, int index)
{
    if (t->items == 0 || t->size * 2 >= t->capacity)
    {
        int* _Owner new_items = malloc(16 * sizeof(int));
        if (new_items == 0)
        {
            return 0;
        }
        if (t->items != 0)
        {
            free(t->items);
        }
        t->items = new_items;
    }
    return t->items[index]; /* the old items are not reached */
}

struct X
{
    int i;
};

int freed_in_if_arrow(int n)
{
    struct X* _Owner p = malloc(sizeof(struct X));
    if (!p)
    {
        return 0;
    }
    struct X* q = p;
    if (n)
    {
        free(p);
        p = 0;
    }
    int r = q->i; //lint 31
    free(p);
    return r;
}

int freed_in_while_with_break_arrow(int n)
{
    struct X* _Owner p = malloc(sizeof(struct X));
    if (!p)
    {
        return 0;
    }
    struct X* q = p;
    while (n)
    {
        free(p);
        p = 0;
        break;
    }
    int r = q->i; //lint 31
    free(p);
    return r;
}

int reassigned_with_the_owner_arrow(int n)
{
    struct X* _Owner p = malloc(sizeof(struct X));
    if (!p)
    {
        return 0;
    }
    struct X* q = p;
    int r = 0;
    if (n)
    {
        free(p);
        p = malloc(sizeof(struct X));
        q = p;
    }
    if (q)
    {
        r = q->i; /* q follows p */
    }
    free(p);
    return r;
}

struct token
{
    int type;
    struct token* _Owner next;
};

struct token_list
{
    struct token* _Owner head;
    struct token* tail;
};

void consume(struct token_list* input_list);

int pointer_replaced_by_a_call_in_loop(struct token_list* input_list)
{
    int n = 0;
    for (;;)
    {
        if (input_list->head == 0)
        {
            return n;
        }
        if (input_list->head->type == 1) /* head was replaced by the call */
        {
            break;
        }
        consume(input_list);
        n++;
    }
    return n;
}

int freed_in_switch_case(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p)
    {
        return 0;
    }
    int* q = p;
    switch (c)
    {
        case 1:
            free(p);
            p = 0;
            break;
        default:
            break;
    }
    int r = *q; //lint 31
    free(p);
    return r;
}

int freed_in_switch_case_arrow(int c)
{
    struct X* _Owner p = malloc(sizeof(struct X));
    if (!p)
    {
        return 0;
    }
    struct X* q = p;
    switch (c)
    {
        case 1:
            free(p);
            p = 0;
            break;
        case 2:
            q->i = 2; /* not freed on this path */
            break;
        default:
            break;
    }
    int r = q->i; //lint 31
    free(p);
    return r;
}

int switch_case_returns(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p)
    {
        return 0;
    }
    int* q = p;
    switch (c)
    {
        case 1:
            free(p);
            return 0;
        default:
            break;
    }
    int r = *q; /* not freed on this path */
    free(p);
    return r;
}
