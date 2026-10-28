#pragma safety enable

/*
    The false side of `L && R` (and the true side of `L || R`) is a merge of
    two branches with no id of its own; the two sides are tagged as a pair so
    a value of one side is not seen on the other (tokenizer.c control_line
    #include).
*/

#include <stdlib.h>

struct token { int type; struct token* _Owner _Opt next; };
struct token_list { struct token* _Owner _Opt head; struct token* _Opt tail; };
void move_front(struct token_list* dest, struct token_list* input);
void prepend(struct token_list* restrict dest, struct token_list* restrict source);
void token_list_destroy(_Opt _Dtor struct token_list* list);
void use(const struct token* _Opt p);

/* Must NOT warn. */

void and_condition(struct token_list* input)
{
    if (input->head == 0) return;
    if (input->head->type != 1 && input->head->type != 2)
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

void or_condition(struct token_list* input)
{
    if (input->head == 0) return;
    if (input->head->type == 1 || input->head->type == 2)
    {
        use(input->head);
    }
    else
    {
        struct token_list p = { 0 };
        move_front(&p, input);
        prepend(input, &p);
        token_list_destroy(&p);
    }
    if (input->head->type == 3)
        use(input->head);
}

struct S { int* _Owner _Opt table; int size; int cap; };

/* an unknown right operand is on both sides: l->table may be non-null here */
void or_unknown_right(struct S* l)
{
    if (l->table == NULL || l->size * 2 >= l->cap)
    {
        if (l->table != NULL)
        {
            free(l->table);
        }
        l->table = NULL;
    }
}

void and_unknown_right(struct S* l)
{
    if (l->table != NULL && l->size * 2 >= l->cap)
    {
        free(l->table);
        l->table = NULL;
    }
    else if (l->table != NULL)
    {
        free(l->table);
        l->table = NULL;
    }
}
