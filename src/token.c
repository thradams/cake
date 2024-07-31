/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

//#pragma safety enable

#include "ownership.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

/*
  PROVISORY - unchecked was removed, now we control flow ownership error with pragma
  TODO review alternatives from Domingo's branch.
*/
#ifdef __CAKE__
#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wdiscard-owner"
#pragma cake diagnostic ignored "-Wmissing-destructor"
#pragma cake diagnostic ignored "-Wnon-owner-move"
#pragma cake diagnostic ignored "-Wnon-owner-to-_Owner-move"
#endif

//#pragma cake diagnostic pop

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
    struct token* _Owner _Opt p = list->head;
    while (p)
    {
        struct token* _Owner _Opt next = p->next;
        p->next = NULL;
        token_delete(p);
        p = next;
    }

    list->head = NULL;
    list->tail = NULL;
}


void token_range_add_show(struct token* first, struct token* last)
{
    for (struct token* _Opt current = first;
        current != last->next;
        current = current->next)
    {
        current->flags = current->flags & ~TK_C_BACKEND_FLAG_HIDE;
    }
}

void token_range_remove_flag(struct token* first, struct token* last, enum token_flags flag)
{
    for (struct token* _Opt current = first;
        current != last->next;
        current = current->next)
    {
        current->flags = current->flags & ~flag;
    }
}

void token_range_add_flag(struct token* first, struct token* last, enum token_flags flag)
{
    for (struct token* _Opt current = first;
        current != last->next;
        current = current->next)
    {
        current->flags |= flag;
    }
}

void token_list_pop_back(struct token_list* list)
{
    if (list->head == NULL)
        return;

    if (list->head == list->tail)
    {
        token_delete(list->head);
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->prev != NULL);
        list->tail = list->tail->prev;
        token_delete(list->tail->next);
        list->tail->next = NULL;
        if (list->tail == list->head)
        {
            list->tail->prev = NULL;
        }
    }

}

void token_list_pop_front(struct token_list* list) /*unchecked*/
{
    if (list->head == NULL)
        return;

    struct token* _Owner _Opt p = list->head;
    //assert(p->prev == NULL);
    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->head = p->next;
    }
    p->next = NULL;
    p->prev = NULL;
    token_delete(p);
}

struct token* _Owner _Opt token_list_pop_front_get(struct token_list* list)
{
    if (list->head == NULL)
        return NULL;

    struct token* _Owner _Opt head = list->head;

    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;

        head->next = NULL;
        head->prev = NULL;

        return head;
    }

    list->head = head->next;
    head->next = NULL;
    head->prev = NULL;

    return head;
}

void token_list_swap(struct token_list* a, struct token_list* b)
{
    struct token_list temp = *a;
    *a = *b;
    *b = temp;
}

void token_delete(struct token* _Owner _Opt p)
{
    if (p)
    {
        /*
         * ownership warning here is about the p->next
         * we need a way to remove only this specific warning
        */
        assert(p->next == NULL);
        free(p->lexeme);
        free(p);
    }
}

void token_list_set_file(struct token_list* list, struct token* filetoken, int line, int col)
{
    struct token* _Opt p = list->head;
    while (p)
    {
        p->token_origin = filetoken;
        p->line = line;
        p->col = col;
        p = p->next;
    }
}

void token_list_destroy(struct token_list* _Obj_owner list)
{
    struct token* _Owner _Opt p = list->head;
    while (p)
    {
        struct token* _Owner _Opt next = p->next;
        p->next = NULL;
        token_delete(p);
        p = next;
    }
}

char* _Owner _Opt token_list_join_tokens(struct token_list* list, bool bliteral)
{
    struct osstream ss = { 0 };
    if (bliteral)
        ss_fprintf(&ss, "\"");
    bool has_space = false;
    struct token* _Opt current = list->head;

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

    char* _Owner _Opt cstr = ss.c_str;
    ss.c_str = NULL; /*MOVED*/

    ss_close(&ss);

    return cstr;
}


void token_list_paste_string_after(struct token_list* list,
    struct token* after,
    const char* s)
{
    struct tokenizer_ctx tctx = { 0 };
    struct token_list l = tokenizer(&tctx, s, NULL, 0, TK_FLAG_FINAL);
    token_list_insert_after(list, after, &l);
    token_list_destroy(&l);
}

void token_list_paste_string_before(struct token_list* list,
    struct token* before,
    const char* s)
{
    struct tokenizer_ctx tctx = { 0 };
    struct token_list l = tokenizer(&tctx, s, NULL, 0, TK_FLAG_FINAL);
    token_list_insert_before(list, before, &l);
    token_list_destroy(&l);
}


void token_list_insert_after(struct token_list* token_list, struct token* after, struct token_list* append_list)
{
    if (append_list->head == NULL)
        return;

    if (after == NULL)
    {
        assert(append_list->tail != NULL);
        assert(append_list->tail->next == NULL);
        append_list->tail->next = token_list->head;
        token_list->head->prev = append_list->tail;

        token_list->head = append_list->head;
        append_list->head->prev = NULL;
    }
    else
    {
        struct token* _Owner _Opt follow = after->next;
        if (token_list->tail == after)
        {
            token_list->tail = append_list->tail;
        }
        else if (token_list->head == after)
        {
        }
        assert(append_list->tail != NULL);
        assert(append_list->tail->next == NULL);
        append_list->tail->next = follow;
        follow->prev = append_list->tail;
        after->next = append_list->head;
        append_list->head->prev = after;

    }

    append_list->head = NULL;
    append_list->tail = NULL;

}

void token_list_insert_before(struct token_list* token_list, struct token* after, struct token_list* append_list)
{
    token_list_insert_after(token_list, after->prev, append_list);
}

struct token* token_list_add(struct token_list* list, struct token* _Owner pnew) /*unchecked*/
{
    /*avoid accidentally being in 2 different lists*/
    assert(pnew->next == NULL);
    assert(pnew->prev == NULL);
    
    if (list->head == NULL)
    {
        pnew->prev = NULL;
        pnew->next = NULL;
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        pnew->prev = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
    assert(list->tail != NULL);
    assert(list->tail->next == NULL);
    return list->tail;

}

int is_digit(const struct stream* p)
{
    /*
     digit : one of
     0 1 2 3 4 5 6 7 8 9
    */
    return (p->current[0] >= '0' && p->current[0] <= '9');
}

bool token_is_identifier_or_keyword(enum token_type t)
{
    switch (t)
    {
    case TK_IDENTIFIER: return true;

    case TK_KEYWORD_AUTO:
    case TK_KEYWORD_BREAK:
    case TK_KEYWORD_CASE:
    case TK_KEYWORD_CONSTEXPR:
    case TK_KEYWORD_CHAR:
    case TK_KEYWORD_CONST:
    case TK_KEYWORD_CONTINUE:
    case TK_KEYWORD_CATCH: /*extension*/
    case TK_KEYWORD_DEFAULT:
    case TK_KEYWORD_DO:
    case TK_KEYWORD_DEFER: /*extension*/
    case TK_KEYWORD_DOUBLE:
    case TK_KEYWORD_ELSE:
    case TK_KEYWORD_ENUM:
    case TK_KEYWORD_EXTERN:
    case TK_KEYWORD_FLOAT:
    case TK_KEYWORD_FOR:
    case TK_KEYWORD_GOTO:
    case TK_KEYWORD_IF:
    case TK_KEYWORD_INLINE:
    case TK_KEYWORD_INT:
    case TK_KEYWORD_LONG:
    case TK_KEYWORD__INT8:
    case TK_KEYWORD__INT16:
    case TK_KEYWORD__INT32:
    case TK_KEYWORD__INT64:

    case TK_KEYWORD_REGISTER:
    case TK_KEYWORD_RESTRICT:
    case TK_KEYWORD_RETURN:
    case TK_KEYWORD_SHORT:
    case TK_KEYWORD_SIGNED:
    case TK_KEYWORD_SIZEOF:

    case TK_KEYWORD_STATIC:
    case TK_KEYWORD_STRUCT:
    case TK_KEYWORD_SWITCH:
    case TK_KEYWORD_TYPEDEF:
    case TK_KEYWORD_TRY: /*extension*/
    case TK_KEYWORD_THROW: /*extension*/
    case TK_KEYWORD_UNION:
    case TK_KEYWORD_UNSIGNED:
    case TK_KEYWORD_VOID:
    case TK_KEYWORD_VOLATILE:
    case TK_KEYWORD_WHILE:

    case TK_KEYWORD__ALIGNAS:
    case TK_KEYWORD__ALIGNOF:
    case TK_KEYWORD__ATOMIC:
        //microsoft
        //KEYWORD__FASTCALL:
        //KEYWORD__STDCALL
        // 
    case TK_KEYWORD__ASM:
        //end microsoft
    case TK_KEYWORD__BOOL:
    case TK_KEYWORD__COMPLEX:
    case TK_KEYWORD__DECIMAL128:
    case TK_KEYWORD__DECIMAL32:
    case TK_KEYWORD__DECIMAL64:
    case TK_KEYWORD__GENERIC:
    case TK_KEYWORD__IMAGINARY:
    case TK_KEYWORD__NORETURN:
    case TK_KEYWORD__STATIC_ASSERT:
    case TK_KEYWORD_ASSERT: /*extension*/
    case TK_KEYWORD__THREAD_LOCAL:

    case TK_KEYWORD_TYPEOF: /*C23*/

    case TK_KEYWORD_TRUE:  /*C23*/
    case TK_KEYWORD_FALSE:  /*C23*/
    case TK_KEYWORD_NULLPTR:  /*C23*/
    case TK_KEYWORD_TYPEOF_UNQUAL: /*C23*/
    case TK_KEYWORD__BITINT /*C23*/:



        /*cake extension*/
    case TK_KEYWORD__OWNER:
    case TK_KEYWORD__OUT:
    case TK_KEYWORD__OBJ_OWNER:
    case TK_KEYWORD__VIEW:
    case TK_KEYWORD__OPT:


        /*extension compile time functions*/
    case TK_KEYWORD_STATIC_DEBUG: /*extension*/
    case TK_KEYWORD_STATIC_DEBUG_EX: /*extension*/
    case TK_KEYWORD_STATIC_STATE: /*extension*/
    case TK_KEYWORD_STATIC_SET: /*extension*/

        /*https://en.cppreference.com/w/cpp/header/type_traits*/

    case TK_KEYWORD_IS_POINTER:
    case TK_KEYWORD_IS_LVALUE:
    case TK_KEYWORD_IS_CONST:
    case TK_KEYWORD_IS_OWNER:
    case TK_KEYWORD_IS_ARRAY:
    case TK_KEYWORD_IS_FUNCTION:
    case TK_KEYWORD_IS_SCALAR:
    case TK_KEYWORD_IS_ARITHMETIC:
    case TK_KEYWORD_IS_FLOATING_POINT:
    case TK_KEYWORD_IS_INTEGRAL:
        return true;
    default:
        break;
    }

    return false;
}


bool token_is_blank(const struct token* p)
{
    return p->type == TK_BEGIN_OF_FILE ||
        p->type == TK_BLANKS ||
        p->type == TK_LINE_COMMENT ||
        p->type == TK_COMMENT;
}

struct token* _Opt token_list_clone_and_add(struct token_list* list, struct token* pnew)
{
    struct token* _Owner _Opt clone = clone_token(pnew);
    return token_list_add(list, clone);
}

void token_list_append_list_at_beginning(struct token_list* dest, struct token_list* source)
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
        assert(source->tail->next == NULL);
        source->tail->next = dest->head;
        dest->head = source->head;
    }

    source->head = NULL;
    source->tail = NULL;
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
        assert(dest->tail != NULL);
        assert(dest->tail->next == NULL);
        dest->tail->next = source->head;
        source->head->prev = dest->tail;
        dest->tail = source->tail;
    }
    source->head = NULL;
    source->tail = NULL;
}


struct token* _Owner _Opt clone_token(struct token* p)
{
    struct token* _Owner _Opt token = calloc(1, sizeof * token);
    if (token)
    {
        char* _Owner _Opt lexeme = strdup(p->lexeme);
        if (lexeme == NULL)
        {
            free(token);
            return NULL;
        }
        *token = *p;
        token->lexeme = lexeme;
        token->next = NULL;
        token->prev = NULL;
    }
    return token;
}


struct token_list token_list_remove_get(struct token_list* list, struct token* first, struct token* last) /*unchecked*/
{

    struct token_list r = { 0 };

    struct token* before_first = first->prev;
    struct token* _Owner _Opt after_last = last->next; /*MOVED*/
    last->next = NULL; /*MOVED*/

    before_first->next = after_last;
    after_last->prev = before_first;

    r.head = (struct token* _Owner)first;
    first->prev = NULL;
    r.tail = last;



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
    struct token* _Opt current = list->head;
    while (current)
    {
        if (current != list->head)
        {
            printf(u8"˰");
            //printf("`");
        }
        print_literal2(current->lexeme);
        COLOR_ESC_PRINT(printf(RESET));
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
        COLOR_ESC_PRINT(printf(LIGHTGREEN));
    else
        COLOR_ESC_PRINT(printf(LIGHTGRAY));
    char buffer0[50] = { 0 };
    snprintf(buffer0, sizeof buffer0, "%d:%d", p_token->line, p_token->col);
    printf("%-6s ", buffer0);
    printf("%-20s ", get_token_name(p_token->type));
    if (p_token->flags & TK_FLAG_MACRO_EXPANDED)
    {
        COLOR_ESC_PRINT(printf(LIGHTCYAN));
    }
    char buffer[50] = { 0 };
    strcat(buffer, "[");
    if (p_token->flags & TK_FLAG_FINAL)
    {
        strcat(buffer, "final ");
    }
    if (p_token->flags & TK_C_BACKEND_FLAG_HIDE)
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
    COLOR_ESC_PRINT(printf(RESET));
}

void print_tokens(struct token* _Opt p_token)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
    struct token* _Opt current = p_token;
    while (current)
    {
        print_token(current);
        current = current->next;
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
    COLOR_ESC_PRINT(printf(RESET));
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
    if (p_token->flags & TK_C_BACKEND_FLAG_HIDE)
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
    struct token* _Opt current = p_token;
    while (current)
    {
        print_token_html(current);
        current = current->next;
    }
    printf("\n</pre>");
}

void print_position(const char* path, int line, int col, bool visual_studio_ouput_format)
{

    if (visual_studio_ouput_format)
    {
        //MSVC format
        printf("%s(%d,%d): ", path ? path : "<>", line, col);
    }
    else
    {
        //GCC format
        printf(WHITE "%s:%d:%d: ", path ? path : "<>", line, col);
    }
}

void print_line_and_token(struct marker* p_marker, bool visual_studio_ouput_format)
{

    const struct token* p_token = p_marker->p_token_caret ? p_marker->p_token_caret : p_marker->p_token_begin;

    if (p_token == NULL)
        return;

    const int line = p_marker->line;

    if (!visual_studio_ouput_format)
        COLOR_ESC_PRINT(printf(RESET));

    char nbuffer[20] = { 0 };
    int n = snprintf(nbuffer, sizeof nbuffer, "%d", line);
    printf(" %s |", nbuffer);


    //lets find the begin of line
    const struct token* p_line_begin = p_token;
    while (p_line_begin && p_line_begin->prev && (p_line_begin->prev->type != TK_NEWLINE && p_line_begin->prev->type != TK_BEGIN_OF_FILE))
    {
        if (p_line_begin->prev == NULL)
            break;
        p_line_begin = p_line_begin->prev;
    }


    const struct token* p_token_begin = p_marker->p_token_begin ? p_marker->p_token_begin : p_marker->p_token_caret;
    const struct token* p_token_end = p_marker->p_token_end ? p_marker->p_token_end : p_marker->p_token_caret;

    //only expand macros if the error is inside
    const bool expand_macro = p_token_begin->flags & TK_FLAG_MACRO_EXPANDED;


    const struct token* _Opt p_item = p_line_begin;
    while (p_item)
    {
        if (!visual_studio_ouput_format)
        {
            if (p_item->flags & TK_FLAG_MACRO_EXPANDED)
                COLOR_ESC_PRINT(printf(DARKGRAY));
        }

        if (!(p_item->flags & TK_FLAG_MACRO_EXPANDED) || expand_macro)
        {
            const char* p = p_item->lexeme;
            while (*p)
            {
                putc(*p, stdout);
                p++;
            }
        }

        if (!visual_studio_ouput_format)
        {
            if (p_item->flags & TK_FLAG_MACRO_EXPANDED)
                COLOR_ESC_PRINT(printf(RESET));
        }

        if (p_item->type == TK_NEWLINE)
            break;
        p_item = p_item->next;
    }

    if (!visual_studio_ouput_format)
        COLOR_ESC_PRINT(printf(RESET));

    if (p_item == NULL) printf("\n");

    printf(" %*s |", n, " ");
    bool complete = false;
    int start_col = 1;
    int end_col = 1;
    bool onoff = false;
    p_item = p_line_begin;
    while (p_item)
    {
        if (p_item == p_token_begin)
        {
            if (!visual_studio_ouput_format)
                COLOR_ESC_PRINT(printf(LIGHTGREEN));
            onoff = true;
            end_col = start_col;
        }

        if (!(p_item->flags & TK_FLAG_MACRO_EXPANDED) || expand_macro)
        {
            const char* p = p_item->lexeme;
            while (*p)
            {

                if (onoff)
                {
                    putc('~', stdout);
                    end_col++;
                }
                else
                {
                    putc(' ', stdout);
                    if (!complete) start_col++;                    
                }
                p++;
            }
        }

        if (p_item->type == TK_NEWLINE)
            break;

        if (p_item == p_token_end)
        {
            complete = true;
            onoff = false;
            if (!visual_studio_ouput_format)
                COLOR_ESC_PRINT(printf(RESET));
        }

        p_item = p_item->next;
    }

    if (!visual_studio_ouput_format)
        COLOR_ESC_PRINT(printf(RESET));

    printf("\n");
    p_marker->start_col =start_col;
    p_marker->end_col =end_col;
}


