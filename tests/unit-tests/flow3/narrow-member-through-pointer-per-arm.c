#pragma safety enable

/*
    input->head has one value per arm of the first if: the old head (alive) on
    one arm, an unknown head on the arm whose loop moved the old one away
    (ended there). Testing input->head->type afterwards narrows each arm's
    member values only for the REF of that arm, so the ended old head does
    not reach the arm where it is alive (tokenizer.c control_line #include).
*/

struct token { int type; struct token* _Owner _Opt next; };
struct token_list { struct token* _Owner _Opt head; struct token* _Opt tail; };
void move_front(struct token_list* dest, struct token_list* input);
void prepend(struct token_list* restrict dest, struct token_list* restrict source);
void token_list_destroy(_Opt _Dtor struct token_list* list);
void use(const struct token* _Opt p);

void include_shape(struct token_list* input)
{
    if (input->head == 0) return;
    if (input->head->type != 1)
    {
        struct token_list p = { 0 };
        while (input->head != 0 && input->head->type != 10)
        {
            move_front(&p, input);
            if (input->head == 0)
            {
                token_list_destroy(&p);
                return;
            }
        }
        prepend(input, &p);
        token_list_destroy(&p);
    }
    if (input->head->type == 2)
        use(input->head);
}

void if_else_member(struct token_list* input)
{
    if (input->head == 0) return;
    if (input->head->type == 1)
    {
        struct token_list p = { 0 };
        move_front(&p, input);
        prepend(input, &p);
        token_list_destroy(&p);
    }
    if (input->head->type != 2)
        use(input->head);
}
