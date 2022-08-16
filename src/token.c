#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "console.h"
#include "osstream.h"
#include "tokenizer.h"


#ifdef _WIN32
#include <crtdbg.h>
#include <Windows.h>
#include <debugapi.h>

#undef assert
#define assert _ASSERTE
#endif
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
        list->head = list->tail = NULL;
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

struct token* token_list_pop_front(struct token_list* list)
{
    if (list->head == NULL)
        return NULL;

    struct token* p = list->head;
    if (list->head == list->tail)
    {
        list->head = list->tail = NULL;
    }
    else
    {
        list->head = list->head->next;
    }
    p->next = NULL;
    p->prev = NULL;
    return p;
}

void token_delete(struct token* p)
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
        p->pFile = filetoken;
        p->line = line;
        p->col = col;
        p = p->next;
    }
}

void token_list_destroy(struct token_list* list)
{
    struct token* p = list->head;
    while (p)
    {
        struct token* next = p->next;
        token_delete(p);
        p = next;
    }
}

char* token_list_join_tokens(struct token_list* list, bool bliteral)
{
    struct osstream ss = { 0 };
    if (bliteral)
        ss_fprintf(&ss, "\"");
    bool has_space = false;
    struct token* pCurrent = list->head;
    
    while (pCurrent)
    {
        if (token_is_blank(pCurrent))
        {
            has_space = true;
            pCurrent = pCurrent->next;
            continue;
        }

        if (has_space)
            ss_fprintf(&ss, " ");

        const char* p = pCurrent->lexeme;
        while (*p)
        {
            if (*p == '"')
              ss_fprintf(&ss, "\\\"");
            else
              ss_fprintf(&ss, "%c", *p);
            p++;
        }
        

        pCurrent = pCurrent->next;
        if (pCurrent)
          has_space = pCurrent->flags & TK_FLAG_HAS_SPACE_BEFORE;
    }

    if (bliteral)
        ss_fprintf(&ss, "\"");

    return ss.c_str;
}

void token_list_insert_after(struct token_list* list, struct token* pAfter, struct token_list* append)
{
    if (append->head == NULL)
        return;

    if (pAfter == NULL)
    {
        append->tail->next = list->head;
        list->head->prev = append->tail;

        list->head = append->head;
        append->head->prev = NULL;
    }
    else
    {
        struct token* pFollow = pAfter->next;
        if (list->tail == pAfter)
        {
            list->tail = append->tail;
        }
        else if (list->head == pAfter)
        {
        }
        append->tail->next = pFollow;
        pAfter->next = append->head;
    }
}

struct token* token_list_add(struct token_list* list, struct token* pnew)
{
    /*evitar que sem querer esteja em 2 listas diferentes*/
    assert(pnew->next == NULL);
    assert(pnew->prev == NULL);

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
    return pnew;
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
        p->type == TK_COMENT;
}

struct token* token_list_clone_and_add(struct token_list* list, struct token* pnew)
{
    struct token* clone = clone_token(pnew);
    token_list_add(list, clone);
    return clone;
}

void token_list_append_list_at_beginning(struct token_list* dest, struct token_list* source)
{
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
}

void token_list_append_list(struct token_list* dest, struct token_list* source)
{
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
}


struct token* clone_token(struct token* p)
{
    struct token* pNew = calloc(1, sizeof * pNew);
    if (pNew)
    {
        //*pNew = *p;
        pNew->flags = p->flags;
        pNew->lexeme = strdup(p->lexeme);
        pNew->type = p->type;
        //pNew->next = NULL;
        //pNew->prev = NULL;
    }
    return pNew;
}

struct token_list token_list_remove(struct token_list* list, struct token* first, struct token* last)
{
    struct token_list r = { 0 };

    struct token* before_first = first->prev;
    struct token* after_last = last->next;

    before_first->next = after_last;
    after_last->prev = before_first;

    r.head = first;
    first->prev = NULL;
    r.tail = last;
    last->next = NULL;

    return r;
}



bool token_list_is_empty(struct token_list* p)
{
    assert((p->head == NULL && p->tail == NULL) ||
           (p->head != NULL && p->tail != NULL));

    return p->head == NULL;
}



void print_list(struct token_list* list)
{
    struct token* pCurrent = list->head;
    while (pCurrent)
    {
        if (pCurrent != list->head)
        {
            printf(u8"˰");
            //printf("`");
        }
        print_literal2(pCurrent->lexeme);
        printf(RESET);
        if (pCurrent == list->tail)
        {
            //printf("`");
        }
        pCurrent = pCurrent->next;
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
    char buffer0[50] = { 0 };
    snprintf(buffer0, sizeof buffer0, "%d:%d", p_token->line, p_token->col);
    printf("%-6s ", buffer0);
    printf("%-20s ", get_token_name(p_token->type));
    if (p_token->flags & TK_FLAG_MACRO_EXPANDED)
    {
        printf(LIGHTCYAN);
    }
    char buffer[50] = { 0 };
    strcat(buffer, "[");
    if (p_token->flags & TK_FLAG_FINAL)
    {
        strcat(buffer, "final ");
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
    struct token* pCurrent = p_token;
    while (pCurrent)
    {
        print_token(pCurrent);
        pCurrent = pCurrent->next;
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
    printf(RESET);
}

