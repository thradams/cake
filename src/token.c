/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable


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
    for (struct token* current = first;
         current != last->next;
         current = current->next)
    {
        current->flags = current->flags & ~TK_C_BACKEND_FLAG_HIDE;
        if (current->next == NULL)
            break;
    }
}

void token_range_remove_flag(struct token* first, struct token* last, enum token_flags flag)
{
    for (struct token* _Opt current = first;
        current && current != last->next;
        current = current->next)
    {
        current->flags = current->flags & ~flag;
    }
}

void token_range_add_flag(struct token* first, struct token* last, enum token_flags flag)
{
    for (struct token* _Opt current = first;
        current && current != last->next;
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
    assert(p->prev == NULL);

    if (list->head == list->tail)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->head = p->next;
        if (list->head)
            list->head->prev = NULL;
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

void token_list_destroy(_Opt _Dtor struct token_list* list)
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


void token_list_insert_after(struct token_list* token_list, struct token* _Opt after, struct token_list* append_list)
{
    if (append_list->head == NULL)
    {
        return;//nothing to append
    }

    if (token_list->head == NULL)
    {
        assert(after == NULL);
        token_list->head = append_list->head;
        token_list->tail = append_list->tail;
        append_list->head = NULL;
        append_list->tail = NULL;
        return;
    }

    if (after == NULL)
    {
        assert(append_list->tail != NULL);
        assert(append_list->tail->next == NULL);
        append_list->tail->next = token_list->head;
        token_list->head->prev = append_list->tail; //TODO empty case

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

bool token_list_is_equal(const struct token_list* list_a, const struct token_list* list_b)
{
    struct token* _Opt p_tka = list_a->head;
    struct token* _Opt p_tkb = list_b->head;

    while (p_tka && p_tkb)
    {
        if (p_tka->type != p_tkb->type)
            return false;

        if (strcmp(p_tka->lexeme, p_tkb->lexeme) != 0)
            return false;

        p_tka = p_tka->next;
        p_tkb = p_tkb->next;
    }

    return p_tka == NULL && p_tkb == NULL;
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
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);

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
    case TK_KEYWORD__CTOR:
    case TK_KEYWORD__DTOR:
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

    if (clone == NULL)
        return NULL;

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
        assert(source->tail != NULL);
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
    _Opt struct token* _Owner _Opt token = calloc(1, sizeof * token);
    if (token == NULL)
        return NULL;

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

    return token;
}


struct token_list token_list_remove_get(struct token_list* list, struct token* first, struct token* last)
{
    /*
       token_list_remove_get removes a range of tokens from a doubly - linked token list and 
       returns them as a new list.  It does not delete the tokens; it just detaches them from 
       the original list.
    */

    struct token_list r = { 0 };

    struct token* _Opt before_first = first->prev;
    struct token* _Owner _Opt after_last = last->next; /*MOVED*/

    if (before_first)
    {
        before_first->next = after_last;
    }
    else
    {
        list->head = last->next;
    }

    if (after_last)
    {
        after_last->prev = before_first;
    }
    else
    {
        list->tail = NULL;
    }

    last->next = NULL; /*MOVED*/

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


void print_token(const struct token* p_token)
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
    printf(RESET);
}

void print_tokens(const struct token* _Opt p_token)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" RESET);
    const struct token* _Opt current = p_token;
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

    try
    {
        const struct token* _Opt p_token = p_marker->p_token_caret ? p_marker->p_token_caret : p_marker->p_token_begin;

        if (p_token == NULL)
            throw;

        const int line = p_marker->line;

        if (!visual_studio_ouput_format)
            printf(RESET);

        char nbuffer[20] = { 0 };
        int n = snprintf(nbuffer, sizeof nbuffer, "%d", line);
        printf(" %s |", nbuffer);


        //lets find the begin of line
        const struct token* p_line_begin = p_token;
        while (p_line_begin->prev && (p_line_begin->prev->type != TK_NEWLINE && p_line_begin->prev->type != TK_BEGIN_OF_FILE))
        {
            p_line_begin = p_line_begin->prev;
        }


        const struct token* _Opt p_token_begin = p_marker->p_token_begin ? p_marker->p_token_begin : p_marker->p_token_caret;
        const struct token* _Opt p_token_end = p_marker->p_token_end ? p_marker->p_token_end : p_marker->p_token_caret;

        if (p_token_begin == NULL)
            throw;


        //only expand macros if the error is inside
        const bool expand_macro = p_token_begin->flags & TK_FLAG_MACRO_EXPANDED;

        if (!visual_studio_ouput_format)
            printf(LIGHTBLUE);

        const struct token* _Opt p_item = p_line_begin;
        while (p_item)
        {
            if (!visual_studio_ouput_format)
            {
                if (p_item->flags & TK_FLAG_MACRO_EXPANDED)
                {
                    printf(DARKGRAY);
                }
                else if (p_item->type >= TK_KEYWORD_AUTO &&
                         p_item->type <= TK_KEYWORD_IS_INTEGRAL)
                {
                    printf(BLUE);
                }
                else if (p_item->type == TK_COMMENT ||
                         p_item->type == TK_LINE_COMMENT)
                {
                    printf(YELLOW);
                }
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
                printf(RESET);
            }

            if (p_item->type == TK_NEWLINE)
                break;
            p_item = p_item->next;
        }

        if (!visual_studio_ouput_format)
            printf(RESET);

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
                    printf(LIGHTGREEN);
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
                    printf(RESET);
            }

            p_item = p_item->next;
        }

        if (!visual_studio_ouput_format)
            printf(RESET);

        printf("\n");
        p_marker->start_col = start_col;
        p_marker->end_col = end_col;
    }
    catch
    {
    }
}

static void digit_sequence_opt(struct stream* stream)
{
    while (is_digit(stream))
    {
        stream_match(stream);
    }
}

static void binary_exponent_part(struct stream* stream)
{
    // p signopt digit - sequence
    // P   signopt digit - sequence

    stream_match(stream); // p or P
    if (stream->current[0] == '+' || stream->current[0] == '-')
    {
        stream_match(stream); // p or P
    }
    digit_sequence_opt(stream);
}

static bool is_hexadecimal_digit(struct stream* stream)
{
    return (stream->current[0] >= '0' && stream->current[0] <= '9') ||
        (stream->current[0] >= 'a' && stream->current[0] <= 'f') ||
        (stream->current[0] >= 'A' && stream->current[0] <= 'F');
}

static bool is_octal_digit(struct stream* stream)
{
    return stream->current[0] >= '0' && stream->current[0] <= '7';
}

static void hexadecimal_digit_sequence(struct stream* stream)
{
    /*
     hexadecimal-digit-sequence:
     hexadecimal-digit
     hexadecimal-digit ’_Opt hexadecimal-digit
    */

    stream_match(stream);
    while (stream->current[0] == '\'' ||
        is_hexadecimal_digit(stream))
    {
        if (stream->current[0] == '\'')
        {
            stream_match(stream);
            if (!is_hexadecimal_digit(stream))
            {
                // erro
            }
            stream_match(stream);
        }
        else
            stream_match(stream);
    }
}

static void integer_suffix_opt(struct stream* stream, char suffix[4])
{
    /*
        (6.4.4.2) integer-suffix:
          unsigned-suffix long-suffixopt
          unsigned-suffix long-long-suffix
          unsigned-suffix bit-precise-int-suffix
          long-suffix unsigned-suffixopt
          long-long-suffix unsigned-suffixopt
          bit-precise-int-suffix unsigned-suffixop
    */

    //test 3100
    if (/*unsigned-suffix*/
        stream->current[0] == 'U' || stream->current[0] == 'u')
    {
        suffix[0] = 'U';
        stream_match(stream);


        /*long-suffixopt*/
        if (stream->current[0] == 'l' || stream->current[0] == 'L')
        {
            suffix[1] = 'L';
            stream_match(stream);
        }

        /*long-long-suffix*/
        if (stream->current[0] == 'l' || stream->current[0] == 'L')
        {
            suffix[2] = 'L';
            stream_match(stream);
        }
    }
    else if ((stream->current[0] == 'l' || stream->current[0] == 'L'))
    {
        suffix[0] = 'L';

        /*long-suffix*/
        stream_match(stream);

        /*long-long-suffix*/
        if ((stream->current[0] == 'l' || stream->current[0] == 'L'))
        {
            suffix[1] = 'L';
            stream_match(stream);
        }

        if (/*unsigned-suffix*/
            stream->current[0] == 'U' || stream->current[0] == 'u')
        {

            //normalize the output from LLU to ul 
            suffix[3] = suffix[2];
            suffix[2] = suffix[1];
            suffix[1] = suffix[0];
            suffix[0] = 'U';
            stream_match(stream);
        }
    }
    ///////////////MICROSOFT ////////////////////////
        //TODO unit test
    else if (stream->current[0] == 'i' &&
             stream->current[1] == '8')
    {
        stream_match(stream);
        stream_match(stream);
        stream_match(stream);
        suffix[0] = 'i';
        suffix[1] = '8';
    }
    else if (stream->current[0] == 'i' &&
             stream->current[1] == '3' &&
             stream->current[2] == '2')
    {
        stream_match(stream);
        stream_match(stream);
        stream_match(stream);
        suffix[0] = 'i';
        suffix[1] = '3';
        suffix[2] = '2';
    }
    else if (stream->current[0] == 'i' &&
             stream->current[1] == '6' &&
             stream->current[2] == '4')
    {
        stream_match(stream);
        stream_match(stream);
        stream_match(stream);
        suffix[0] = 'i';
        suffix[1] = '6';
        suffix[2] = '4';
    }
    ///////////////MICROSOFT ////////////////////////
}

static void exponent_part_opt(struct stream* stream)
{
    /*
    exponent-part:
    e signopt digit-sequence
    E signopt digit-sequence
    */
    if (stream->current[0] == 'e' || stream->current[0] == 'E')
    {
        stream_match(stream);

        if (stream->current[0] == '-' || stream->current[0] == '+')
        {
            stream_match(stream);
        }
        digit_sequence_opt(stream);
    }
}

static void floating_suffix_opt(struct stream* stream, char suffix[4])
{

    if (stream->current[0] == 'l' || stream->current[0] == 'L')
    {
        suffix[0] = 'L';
        stream_match(stream);
    }
    else if (stream->current[0] == 'f' || stream->current[0] == 'F')
    {
        suffix[0] = 'F';
        stream_match(stream);
    }
}

static bool is_binary_digit(struct stream* stream)
{
    return stream->current[0] >= '0' && stream->current[0] <= '1';
}

static bool is_nonzero_digit(struct stream* stream)
{
    return stream->current[0] >= '1' && stream->current[0] <= '9';
}

enum token_type parse_number_core(struct stream* stream, char suffix[4], _Ctor char errmsg[100])
{
    errmsg[0] = '\0';

    enum token_type type = TK_NONE;
    if (stream->current[0] == '.')
    {
        type = TK_COMPILER_DECIMAL_FLOATING_CONSTANT;
        stream_match(stream);

        if (stream->current[0] == '.')
        {
            snprintf(errmsg, 100, "too many decimal points in number");
            return TK_NONE;
        }

        digit_sequence_opt(stream);
        exponent_part_opt(stream);
        floating_suffix_opt(stream, suffix);
    }
    else if (stream->current[0] == '0' && (stream->current[1] == 'x' || stream->current[1] == 'X'))
    {
        type = TK_COMPILER_HEXADECIMAL_CONSTANT;

        stream_match(stream);
        stream_match(stream);

        if (is_hexadecimal_digit(stream))
        {
            while (is_hexadecimal_digit(stream))
            {
                stream_match(stream);
            }
        }
        else
        {
            snprintf(errmsg, 100, "expected hexadecimal digit");
            return TK_NONE;
        }

        integer_suffix_opt(stream, suffix);

        if (stream->current[0] == '.')
        {
            type = TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
            hexadecimal_digit_sequence(stream);
        }

        if (stream->current[0] == 'p' ||
            stream->current[0] == 'P')
        {
            type = TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT;
            binary_exponent_part(stream);
        }

        if (type == TK_COMPILER_HEXADECIMAL_FLOATING_CONSTANT)
        {
            floating_suffix_opt(stream, suffix);
        }
    }
    else if (stream->current[0] == '0' && (stream->current[1] == 'b' || stream->current[1] == 'B'))
    {
        type = TK_COMPILER_BINARY_CONSTANT;
        stream_match(stream);
        stream_match(stream);
        if (is_binary_digit(stream))
        {
            while (is_binary_digit(stream))
            {
                stream_match(stream);
            }
        }
        else
        {
            snprintf(errmsg, 100, "expected binary digit");
            return TK_NONE;
        }
        integer_suffix_opt(stream, suffix);
    }
    else if (stream->current[0] == '0') // octal
    {
        type = TK_COMPILER_OCTAL_CONSTANT;

        stream_match(stream);

        if (stream->current[0] == 'O' || stream->current[0] == 'o')
        {
            //C2Y
            //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3319.htm
            stream_match(stream);
        }

        while (is_octal_digit(stream))
        {
            stream_match(stream);
        }
        integer_suffix_opt(stream, suffix);

        if (stream->current[0] == '.')
        {
            type = TK_COMPILER_DECIMAL_FLOATING_CONSTANT;
            hexadecimal_digit_sequence(stream);
            floating_suffix_opt(stream, suffix);
        }
    }
    else if (is_nonzero_digit(stream)) // decimal
    {
        type = TK_COMPILER_DECIMAL_CONSTANT;

        stream_match(stream);
        while (is_digit(stream))
        {
            stream_match(stream);
        }
        integer_suffix_opt(stream, suffix);

        if (stream->current[0] == 'e' || stream->current[0] == 'E')
        {
            exponent_part_opt(stream);
            floating_suffix_opt(stream, suffix);

        }
        else if (stream->current[0] == '.')
        {
            stream_match(stream);
            type = TK_COMPILER_DECIMAL_FLOATING_CONSTANT;

            if (stream->current[0] == '.')
            {
                snprintf(errmsg, 100, "too many decimal points in number");
                return TK_NONE;
            }

            digit_sequence_opt(stream);

            exponent_part_opt(stream);
            floating_suffix_opt(stream, suffix);
        }
    }

    return type;
}

enum token_type parse_number(const char* lexeme, char suffix[4], _Ctor char errmsg[100])
{
    struct stream stream = {
        .source = lexeme,
        .current = lexeme,
        .line = 1,
        .col = 1,
        .path = "",
    };

    return parse_number_core(&stream, suffix, errmsg);
}

/*
    https://en.wikipedia.org/wiki/UTF-8
    Since the restriction of the Unicode code-space to 21-bit values in 2003,
    UTF-8 is defined to encode code points in one to four bytes, depending on the number
    of significant bits in the numerical value of the code point. The following table shows
    the structure of the encoding. The x characters are replaced by the bits of the code point.

    Code point <->UTF - 8 conversion
    First         | Last           | Byte 1   | Byte 2   | Byte 3   | Byte 4
    --------------| -------------- |----------|----------|----------| ----------
    U+0000      0 | U+007F     127 | 0xxxxxxx |          |          |
    U+0080    128 | U+07FF    2047 | 110xxxxx | 10xxxxxx |          |
    U+0800   2048 | U+FFFF   65535 | 1110xxxx | 10xxxxxx | 10xxxxxx |
    U+10000 65536 | U+10FFFF 69631 | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx
*/

const unsigned char* _Opt utf8_decode(const unsigned char* s, _Ctor unsigned int* c)
{
    *c = 0; //out

    if (s[0] == '\0')
    {
        *c = 0;
        return NULL; /*end*/
    }

    const unsigned char* _Opt next = NULL;
    if (s[0] < 0x80)
    {
        *c = s[0];
        assert(*c >= 0x0000 && *c <= 0x007F);
        next = s + 1;
    }
    else if ((s[0] & 0xe0) == 0xc0)
    {
        *c = ((int)(s[0] & 0x1f) << 6) |
            ((int)(s[1] & 0x3f) << 0);
        assert(*c >= 0x0080 && *c <= 0x07FF);
        next = s + 2;
    }
    else if ((s[0] & 0xf0) == 0xe0)
    {
        *c = ((int)(s[0] & 0x0f) << 12) |
            ((int)(s[1] & 0x3f) << 6) |
            ((int)(s[2] & 0x3f) << 0);
        assert(*c >= 0x0800 && *c <= 0xFFFF);
        next = s + 3;
    }
    else if ((s[0] & 0xf8) == 0xf0 && (s[0] <= 0xf4))
    {
        *c = ((int)(s[0] & 0x07) << 18) |
            ((int)(s[1] & 0x3f) << 12) |
            ((int)(s[2] & 0x3f) << 6) |
            ((int)(s[3] & 0x3f) << 0);
        assert(*c >= 0x10000 && *c <= 0x10FFFF);
        next = s + 4;
    }
    else
    {
        *c = 0;      // invalid
        next = s + 1; // skip this byte
    }

    if (*c >= 0xd800 && *c <= 0xdfff)
    {
        *c = 0; // surrogate half
    }

    return next;
}

static bool is_hex_digit(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return true;
    else if (c >= 'a' && c <= 'f')
        return true;
    else if (c >= 'A' && c <= 'F')
        return true;
    return false;
}

const unsigned char* _Opt escape_sequences_decode_opt(const unsigned char* p, unsigned int* out_value)
{
    // TODO OVERFLOW CHECK
    if (*p == 'x')
    {
        p++;
        int result = 0;
        while (is_hex_digit(*p))
        {
            int byte = 0;
            if (*p >= '0' && *p <= '9')
                byte = (*p - '0');
            else if (*p >= 'a' && *p <= 'f')
                byte = (*p - 'a') + 10;
            else if (*p >= 'A' && *p <= 'F')
                byte = (*p - 'A') + 10;

            result = (result << 4) | (byte & 0xF);
            p++;
        }

        *out_value = result;
    }
    else if (*p == 'u' || *p == 'U')
    {
        // TODO  assuming input is checked
        // missing tests
        const int num_of_hex_digits = *p == 'U' ? 8 : 4;

        p++;
        unsigned long long result = 0;
        for (int i = 0; i < num_of_hex_digits; i++)
        {
            int byte = 0;
            if (*p >= '0' && *p <= '9')
                byte = (*p - '0');
            else if (*p >= 'a' && *p <= 'f')
                byte = (*p - 'a') + 10;
            else if (*p >= 'A' && *p <= 'F')
                byte = (*p - 'A') + 10;

            result = (result << 4) | (byte & 0xF);
            p++;
        }

        *out_value = (int)result;
    }
    else if (*p == '0')
    {
        // octal digit
        p++;

        int result = 0;
        while ((*p >= '0' && *p <= '7'))
        {
            int byte;
            byte = (*p - '0');
            result = (result << 4) | (byte & 0xF);
            p++;
        }
        *out_value = result;
    }
    else
    {
        switch (*p)
        {
        case 'a':
            *out_value = '\a';
            break;
        case 'b':
            *out_value = '\b';
            break;
        case 'f':
            *out_value = '\f';
            break;
        case 'n':
            *out_value = '\n';
            break;
        case 'r':
            *out_value = '\r';
            break;
            ;
        case 't':
            *out_value = '\t';
            break;
        case '\'':
            *out_value = '\'';
            break;
        case '\\':
            *out_value = '\\';
            break;
        case '"':
            *out_value = '"';
            break;
        default:
            // this is handled at tokenizer
            assert(false);
            return NULL;
        }
        p++;
    }

    return p;
}

#ifdef TEST

void token_list_remove_get_test()
{
    struct token_list list = { 0 };
    struct token* pnew = calloc(1, sizeof * pnew);
    token_list_add(&list, pnew);
    struct token_list r = token_list_remove_get(&list, pnew, pnew);
    assert(list.head == NULL);
    assert(list.tail == NULL);
}

void token_list_remove_get_test2()
{
    struct token_list list = { 0 };
    struct token* pnew1 = calloc(1, sizeof * pnew1);
    token_list_add(&list, pnew1);
    struct token* pnew2 = calloc(1, sizeof * pnew2);
    token_list_add(&list, pnew2);

    struct token_list r = token_list_remove_get(&list, pnew1, pnew1);
    assert(list.head == pnew2);
    assert(list.tail == pnew2);
}


#endif