#pragma safety enable

/*
    The tokenizer.c embed_parse_parameters '(' clause: the head is tested
    before the loop, then consumed in it. The loop merge retags the origin of
    the values of *input->head and keeps their same-object side only as the
    narrowed-from map; the narrowing and the read through input->head must
    look at it too, or the old head of the side input == dest (ended by the
    call) comes back on the side input != dest.
*/

struct token { int type; struct token* _Owner next; };
struct token_list { struct token* _Owner head; struct token* tail; };
void consume(struct token_list* _Opt dest, struct token_list* input);
void use(const struct token* p);

void tested_before_the_loop(struct token_list* _Opt dest, struct token_list* input)
{
    if (input->head)
    {
        consume(dest, input);
        for (;;)
        {
            if (input->head == 0) return;
            use(input->head);
            consume(dest, input);
        }
    }
}
