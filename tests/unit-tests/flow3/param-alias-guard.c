#pragma safety enable

void free(void* _Owner _Opt p);

struct X { int i; };

/* with p == p2 the function returns early: no warning 100 */
void f(struct X* _Owner p, struct X* p2)
{
    if (p == p2)
    {
        free(p);
        return;
    }
    free(p);
    p2->i = 1;
}

/* the shape of preprocessor_constant_expression (tokenizer.c) */
struct token { struct token* _Owner next; };
struct token_list { struct token* _Owner head; struct token* tail; };
void token_list_swap(struct token_list* a, struct token_list* b);
void token_list_destroy(_Dtor struct token_list* list);
void report(const struct token* p);

void h(int* ctx, struct token_list* output_list, struct token_list* input_list)
{
    if (output_list == input_list)
        return;

    struct token* first = input_list->head;
    struct token_list r = { 0 };
    token_list_swap(output_list, &r);
    if (first)
        report(first);
    token_list_destroy(&r);
    (void)ctx;
}

/* bisect h: without the first parameter */
void h1(struct token_list* output_list, struct token_list* input_list)
{
    if (output_list == input_list)
        return;

    struct token* first = input_list->head;
    struct token_list r = { 0 };
    token_list_swap(output_list, &r);
    if (first)
        report(first);
    token_list_destroy(&r);
}

/* bisect h: the guard alone, then only a read */
void h2(struct token_list* output_list, const struct token_list* input_list)
{
    if (output_list == input_list)
        return;

    struct token_list r = { 0 };
    token_list_swap(output_list, &r);
    if (input_list->head)
        report(input_list->head);
    token_list_destroy(&r);
}

/* the comparison alone, to see if it is known when aliased */
int g(struct X* p, struct X* p2)
{
    if (p == p2)
        return 1;
    return 0;
}
