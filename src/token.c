#include "ownership.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "console.h"
#include "osstream.h"
#include "tokenizer.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#endif

#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif

bool style_has_space(const struct token* token)
{
    return token_is_blank(token->prev);
}

bool style_has_one_space(const struct token* token)
{
    return token->prev &&
        token->prev->type == TK_BLANKS;
}

void print_literal2(const char* s);

void token_list_clear(struct token_list* list)
{
    token_list_destroy(list);
    list->head = NULL;
    list->tail = NULL;
}


void token_range_add_show(struct token* first, struct token* last)
{
    for (struct token* current = first;
        current != last->next;
        current = current->next)
    {
        current->flags = current->flags & ~TK_FLAG_HIDE;
    }
}

void token_range_remove_flag(struct token* first, struct token* last, enum token_flags flag)
{
    for (struct token* current = first;
        current != last->next;
        current = current->next)
    {
        current->flags = current->flags & ~flag;
    }
}

void token_range_add_flag(struct token* first, struct token* last, enum token_flags flag)
{
    for (struct token* current = first;
        current != last->next;
        current = current->next)
    {
        current->flags |= flag;
    }
}

struct token* token_list_pop_back(struct token_list* list)
{
    if (list->head == NULL)
        return NULL;

    struct token* p = list->tail;
    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
        if (list->tail == list->head)
        {
            list->tail->prev = NULL;
        }
    }
    p->next = NULL;
    p->prev = NULL;
    return p;
}

void token_list_pop_front(struct token_list* list)
{
    if (list->head == NULL)
        return;

    struct token* owner p = move list->head;

    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->head = move list->head->next;
    }
    p->next = NULL;
    p->prev = NULL;

    token_delete(p);    
}

struct token* owner token_list_pop_front_get(struct token_list* list)
{
    if (list->head == NULL)
        return NULL;

    struct token* owner p = move list->head;

    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->head = move list->head->next;
    }
    p->next = NULL;
    p->prev = NULL;

    return p;
}

void token_delete(implicit struct token* owner p)
{
    if (p)
    {
        free(p->lexeme);
        free(p);
    }
}

void token_list_set_file(struct token_list* list, struct token* filetoken, int line, int col)
{
    //assert(filetoken != NULL);
    struct token* p = list->head;
    while (p)
    {
        p->token_origin = filetoken;
        p->line = line;
        p->col = col;
        p = p->next;
    }
}

void token_list_destroy(implicit struct token_list* obj_owner list)
{
    struct token* owner p = move list->head;
    while (p)
    {
        struct token* owner next = move p->next;
        token_delete(p);
        p = move next;
    }
}

char* owner token_list_join_tokens(struct token_list* list, bool bliteral)
{
    struct osstream ss = {0};
    if (bliteral)
        ss_fprintf(&ss, "\"");
    bool has_space = false;
    struct token* current = list->head;

    while (current)
    {
        if (token_is_blank(current))
        {
            has_space = true;
            current = current->next;
            continue;
        }

        if (has_space)
            ss_fprintf(&ss, " ");

        const char* p = current->lexeme;
        while (*p)
        {
            if (*p == '"')
                ss_fprintf(&ss, "\\\"");
            else
                ss_fprintf(&ss, "%c", *p);
            p++;
        }


        current = current->next;
        if (current)
            has_space = current->flags & TK_FLAG_HAS_SPACE_BEFORE;
    }

    if (bliteral)
        ss_fprintf(&ss, "\"");

    char* owner cstr = move ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}

void token_list_insert_after(struct token_list* token_list, struct token* after, struct token_list* obj_owner append_list)
{
    if (append_list->head == NULL)
        return;

    if (after == NULL)
    {
        append_list->tail->next = move token_list->head;
        token_list->head->prev = append_list->tail;

        token_list->head = move append_list->head;
        append_list->head->prev = NULL;
    }
    else
    {
        struct token* owner follow = move after->next;
        if (token_list->tail == after)
        {
            token_list->tail = append_list->tail;
        }
        else if (token_list->head == after)
        {
        }
        append_list->tail->next = move follow;
        follow->prev = append_list->tail;
        after->next = move append_list->head;
        append_list->head->prev = after;

    }
}

struct token* token_list_add(struct token_list* list, struct token* owner pnew)
{
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignore "-Wexplicit-move"

    /*evitar que sem querer esteja em 2 listas diferentes*/
    assert(pnew->next == NULL);
    assert(pnew->prev == NULL);
    //assert(pnew->token_origin);

    if (list->head == NULL)
    {
        pnew->prev = NULL;
        pnew->next = NULL;
        list->head = pnew;
        list->tail = pnew;
        //pnew->prev = list->tail;
    }
    else
    {
        pnew->prev = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
    assert(list->tail->next == NULL);
    return list->tail;

#pragma CAKE diagnostic pop
}

int is_digit(struct stream* p)
{
    /*
     digit : one of
     0 1 2 3 4 5 6 7 8 9
    */
    return (p->current[0] >= '0' && p->current[0] <= '9');
}

bool token_is_blank(struct token* p)
{
    return p->type == TK_BEGIN_OF_FILE ||
        p->type == TK_BLANKS ||
        p->type == TK_LINE_COMMENT ||
        p->type == TK_COMMENT;
}

struct token* token_list_clone_and_add(struct token_list* list, struct token* pnew)
{
    struct token* owner clone = clone_token(pnew);
    return token_list_add(list, move clone);
}

void token_list_append_list_at_beginning(struct token_list* dest, struct token_list* obj_owner source)
{
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignore "-Wexplicit-move"

    //print_list(source);
    //printf("->");
    //print_list(dest);
    if (source->head == NULL)
    {
        return;
    }
    if (dest->head == NULL)
    {
        dest->head = source->head;
        dest->tail = source->tail;
    }
    else
    {
        //dest->tail = source->head;
        source->tail->next = dest->head;
        dest->head = source->head;
    }
    //print_list(dest);
#pragma CAKE diagnostic pop
}

void token_list_append_list(struct token_list* dest, struct token_list* obj_owner source)
{
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignore "-Wexplicit-move"

    if (source->head == NULL)
    {
        return;
    }
    if (dest->head == NULL)
    {
        dest->head = source->head;
        dest->tail = source->tail;
    }
    else
    {
        dest->tail->next = source->head;
        source->head->prev = dest->tail;
        dest->tail = source->tail;
    }
#pragma CAKE diagnostic pop
}


struct token* owner clone_token(struct token* p)
{
    struct token* owner token = calloc(1, sizeof * token);
    if (token)
    {
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignore "-Wexplicit-move"
        * token = *p;
#pragma CAKE diagnostic pop

        token->lexeme = move strdup(p->lexeme);
        token->next = NULL;
        token->prev = NULL;
    }
    return token;
}

struct token_list token_list_remove_get(struct token_list* list, struct token* first, struct token* last)
{

    struct token_list r = {0};

    struct token* before_first = first->prev;
    struct token* owner after_last = move last->next;

    before_first->next = move after_last;
    after_last->prev = before_first;

    r.head = move (struct token* owner)first;
    first->prev = NULL;
    r.tail = last;
    last->next = NULL;


    return r;
}


void token_list_remove(struct token_list* list, struct token* first, struct token* last)
{
    struct token_list r = token_list_remove_get(list, first, last);
    token_list_destroy(&r);    
}


bool token_list_is_empty(struct token_list* p)
{
    assert((p->head == NULL && p->tail == NULL) ||
        (p->head != NULL && p->tail != NULL));

    return p->head == NULL;
}



void print_list(struct token_list* list)
{
    struct token* current = list->head;
    while (current)
    {
        if (current != list->head)
        {
            printf(u8"˰");
            //printf("`");
        }
        print_literal2(current->lexeme);
        printf(RESET);
        if (current == list->tail)
        {
            //printf("`");
        }
        current = current->next;
    }
    printf(u8"\n");
}

void print_literal2(const char* s)
{
    if (s == NULL)
    {
        //printf("`");
        //printf("`");
        return;
    }
    //printf("`");
    while (*s)
    {
        switch (*s)
        {
            case '\n':
                printf("\\n");
                break;
            default:
                printf("%c", *s);
        }
        s++;
    }
    //printf("`");
}


void print_token(struct token* p_token)
{
    for (int i = 0; i < p_token->level; i++)
    {
        printf("  ");
    }
    if (p_token->flags & TK_FLAG_FINAL)
        printf(LIGHTGREEN);
    else
        printf(LIGHTGRAY);
    char buffer0[50] = {0};
    snprintf(buffer0, sizeof buffer0, "%d:%d", p_token->line, p_token->col);
    printf("%-6s ", buffer0);
    printf("%-20s ", get_token_name(p_token->type));
    if (p_token->flags & TK_FLAG_MACRO_EXPANDED)
    {
        printf(LIGHTCYAN);
    }
    char buffer[50] = {0};
    strcat(buffer, "[");
    if (p_token->flags & TK_FLAG_FINAL)
    {
        strcat(buffer, "final ");
    }
    if (p_token->flags & TK_FLAG_HIDE)
    {
        strcat(buffer, "hide ");
    }
    if (p_token->flags & TK_FLAG_MACRO_EXPANDED)
    {
        strcat(buffer, "expanded ");
    }
    if (p_token->flags & TK_FLAG_HAS_SPACE_BEFORE)
    {
        strcat(buffer, "space ");
    }
    if (p_token->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
    {
        strcat(buffer, "newline ");
    }
    strcat(buffer, "]");
    printf("%-20s ", buffer);
    print_literal2(p_token->lexeme);
    printf("\n");
    printf(RESET);
}

void print_tokens(struct token* p_token)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
    struct token* current = p_token;
    while (current)
    {
        print_token(current);
        current = current->next;
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
    printf(RESET);
}


void print_token_html(struct token* p_token)
{
    printf("<span class=\"");


    if (!(p_token->flags & TK_FLAG_FINAL))
    {
        printf("notfinal ");
    }

    if (p_token->flags & TK_FLAG_FINAL)
    {
        printf("final ");
    }
    if (p_token->flags & TK_FLAG_HIDE)
    {
        printf("hide ");
    }
    if (p_token->flags & TK_FLAG_MACRO_EXPANDED)
    {
        printf("expanded ");
    }
    if (p_token->flags & TK_FLAG_HAS_SPACE_BEFORE)
    {
        printf("space ");
    }
    if (p_token->flags & TK_FLAG_HAS_NEWLINE_BEFORE)
    {
        printf("newline ");
    }

    printf("\">");

    print_literal2(p_token->lexeme);

    printf("</span>");

    if (p_token->type == TK_NEWLINE || p_token->type == TK_BEGIN_OF_FILE)
    {
        printf("<br>\n");
    }
}

/*
 CSS for html ouput

 <style>
        .final {
          color:blue;
        }

        .notfinal {
          color:gray;
        }

        .hide {
          text-decoration: line-through;
          color:red;
        }

        .expanded {
           background-color:yellow;
        }

        span {
            border-style: solid;
            border-color: gray;
            border-width: 1px 1px;
            padding:1px;
            margin:2px;
        }

</style>

*/
void print_tokens_html(struct token* p_token)
{
    printf("<pre>\n");
    struct token* current = p_token;
    while (current)
    {
        print_token_html(current);
        current = current->next;
    }
    printf("\n</pre>");
}

void print_position(const char* path, int line, int col)
{
    //GCC format
    printf(WHITE "%s:%d:%d: ", path ? path : "<>", line, col);

    //MSVC format
#if 0
    printf(WHITE "%s(%d,%d): ", path ? path : "<>", line, col);
#endif

}

void print_line_and_token(const struct token* p_token)
{
    if (p_token == NULL)
        return;

    int line = p_token->line;
    printf(LIGHTGRAY);

    char nbuffer[20] = {0};
    int n = snprintf(nbuffer, sizeof nbuffer, "%d", line);
    printf(" %s |", nbuffer);

    const struct token* prev = p_token;
    while (prev && prev->prev && (prev->prev->type != TK_NEWLINE && prev->prev->type != TK_BEGIN_OF_FILE))
    {
        prev = prev->prev;
    }
    const struct token* next = prev;
    while (next && (next->type != TK_NEWLINE && next->type != TK_BEGIN_OF_FILE))
    {
        if (next->flags & TK_FLAG_MACRO_EXPANDED)
        {
            if (next->flags & TK_FLAG_HAS_SPACE_BEFORE)
            {
                printf(" ");
            }
        }
        if (next->flags & TK_FLAG_MACRO_EXPANDED)
        {
            printf(DARKGRAY "%s" RESET, next->lexeme);
        }
        else
            printf("%s", next->lexeme);

        next = next->next;
    }
    printf("\n");
    printf(LIGHTGRAY);
    printf(" %*s |", n, " ");
    if (p_token)
    {
        for (int i = 1; i <= (p_token->col - 1); i++)
        {
            printf(" ");
        }
    }

    printf(LIGHTGREEN);

    printf("^");

    char* p = p_token->lexeme + 1;
    while (p && *p)
    {
        printf("~");
        p++;
    }

    printf(RESET);
    printf("\n");
}
