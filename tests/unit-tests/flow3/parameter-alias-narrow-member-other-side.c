#pragma safety enable

/* on the side input == d, input points to *d: testing input->head narrows *d's head, and the head of *input (not reached on that side) keeps its side; it must not come back as the token the call ended */

struct token { int type; struct token* _Owner _Opt next; };
struct token_list { struct token* _Owner _Opt head; struct token* _Opt tail; };
void consume(struct token_list* dest, struct token_list* input);
void consume_opt(struct token_list* _Opt dest, struct token_list* input);
void consume_restrict(struct token_list* restrict dest, struct token_list* restrict input);
void use(const struct token* p);
void token_delete(struct token* _Owner _Opt p);

/* Must NOT warn. */

void loop_eq(struct token_list* d, struct token_list* input)
{
    for (;;)
    {
        consume(d, input);
        if (input->head == 0) return;
        use(input->head);
    }
}

void loop_ne(struct token_list* d, struct token_list* input)
{
    for (;;)
    {
        consume(d, input);
        if (input->head != 0)
            use(input->head);
        else
            return;
    }
}

void loop_eq_null_first(struct token_list* d, struct token_list* input)
{
    for (;;)
    {
        consume(d, input);
        if (0 == input->head) return;
        use(input->head);
    }
}

void loop_type_test(struct token_list* d, struct token_list* input)
{
    /* the embed_parse_parameters shape */
    for (;;)
    {
        consume(d, input);
        if (input->head == 0) return;
        if (input->head->type == 1) return;
        use(input->head);
        consume(d, input);
    }
}

void loop_while(struct token_list* d, struct token_list* input)
{
    while (input->head != 0)
    {
        use(input->head);
        consume(d, input);
    }
}

void loop_opt_dest(struct token_list* _Opt d, struct token_list* input)
{
    for (;;)
    {
        consume_opt(d, input);
        if (input->head == 0) return;
        use(input->head);
    }
}

void loop_restrict(struct token_list* restrict d, struct token_list* restrict input)
{
    for (;;)
    {
        consume_restrict(d, input);
        if (input->head == 0) return;
        use(input->head);
    }
}

void no_loop_twice(struct token_list* d, struct token_list* input)
{
    consume(d, input);
    if (input->head == 0) return;
    use(input->head);
    consume(d, input);
    if (input->head == 0) return;
    use(input->head);
}

void through_d(struct token_list* d, struct token_list* input)
{
    for (;;)
    {
        consume(d, input);
        if (d->head == 0) return;
        use(d->head);
    }
}

/* Must warn: the head really ended (after a call it is missed: failing/parameter-alias-ended-member-after-call.c) */

void ended_then_used(struct token_list* input)
{
    if (input->head == 0) return;
    token_delete(input->head);
    use(input->head); //lint 31
} //lint 72
