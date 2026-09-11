#pragma safety enable
#include "ownership.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "json.h"
#include "error.h"

#define JSON_MAX_DEPTH 200

struct parser
{
    const char *start;
    const char *p;
    int depth;
    struct json_error *error;
};

struct buffer
{
    char * _Owner _Opt data;
    size_t length;
    size_t capacity;
};

static int json_buffer_reserve(struct buffer *b, size_t extra)
{
    int result = 0;
    size_t needed = b->length + extra + 1;
    char * _Opt _Owner data = NULL;

    try
    {
        if (needed <= b->capacity)
        {
            result = 1;
        }
        else
        {
            size_t capacity = b->capacity ? b->capacity : 16;
            while (capacity < needed)
            {
                capacity *= 2;
            }

            data = realloc(b->data, capacity);
            if (data == NULL)
            {
                throw;
            }

            b->data = data; //lint 26
            b->capacity = capacity;
            result = 1;
        }
    }
    catch
    {
    }

    return result;
}

static int json_buffer_push(struct buffer *b, char c)
{
    int result = 0;

    try
    {
        if (!json_buffer_reserve(b, 1))
        {
            throw;
        }

        b->data[b->length++] = c;
        result = 1;
    }
    catch
    {
    }

    return result;
}

static int json_buffer_push_codepoint(struct buffer *b, unsigned long cp)
{
    int result = 0;

    try
    {
        if (!json_buffer_reserve(b, 4))
        {
            throw;
        }

        if (cp < 0x80)
        {
            b->data[b->length++] = (char)cp;
        }
        else if (cp < 0x800)
        {
            b->data[b->length++] = (char)(0xC0 | (cp >> 6));
            b->data[b->length++] = (char)(0x80 | (cp & 0x3F));
        }
        else if (cp < 0x10000)
        {
            b->data[b->length++] = (char)(0xE0 | (cp >> 12));
            b->data[b->length++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            b->data[b->length++] = (char)(0x80 | (cp & 0x3F));
        }
        else
        {
            b->data[b->length++] = (char)(0xF0 | (cp >> 18));
            b->data[b->length++] = (char)(0x80 | ((cp >> 12) & 0x3F));
            b->data[b->length++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            b->data[b->length++] = (char)(0x80 | (cp & 0x3F));
        }
        result = 1;
    }
    catch
    {
    }

    return result;
}

static void json_buffer_free(_Dtor struct buffer *b)
{
    free(b->data);
}

static void json_fail(const struct parser *parser, const char *message)
{
    /* Keep the first error: it is the most specific one. */
    if (parser->error->message[0] == 0)
    {
        const char *scan;
        size_t line = 1;
        size_t column = 1;

        for (scan = parser->start; scan < parser->p; scan++)
        {
            if (*scan == '\n')
            {
                line++;
                column = 1;
            }
            else
            {
                column++;
            }
        }

        snprintf(parser->error->message, sizeof parser->error->message, "%s", message);
        parser->error->line = line;
        parser->error->column = column;
    }
}

static void json_skip_whitespace(struct parser *parser)
{
    while (*parser->p == ' ' || *parser->p == '\t' ||
         *parser->p == '\n' || *parser->p == '\r')
    {
        parser->p++;
    }
}

static void json_append_child(struct json_value *parent, struct json_value * _Owner child)
{
    if (parent->first_child == NULL)
    {
        parent->first_child = child;
    }
    else
    {
        _Assert(parent->last_child);
        _Assert(parent->last_child->next == NULL);
        parent->last_child->next = child;
    }
    parent->last_child = child;
}

static int json_hex_digit(char c)
{
    int result = -1;

    if (c >= '0' && c <= '9')
    {
        result = c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        result = c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F')
    {
        result = c - 'A' + 10;
    }

    return result;
}

/* Reads exactly four hex digits. Returns -1 on malformed input. */
static long json_read_hex4(struct parser *parser)
{
    long result = -1;
    long value = 0;
    int i;

    for (i = 0; i < 4; i++)
    {
        int digit = json_hex_digit(parser->p[i]);
        if (digit < 0)
        {
            break;
        }
        value = value * 16 + digit;
    }

    if (i == 4)
    {
        parser->p += 4;
        result = value;
    }

    return result;
}

/* Parses a quoted string, decoding escapes. Returns an owned string. */
static char * _Opt _Owner json_parse_string(struct parser *parser)
{
    char * _Opt _Owner result = NULL;
    struct buffer buffer = {0};

    try
    {
        if (*parser->p != '"')
        {
            json_fail(parser, "expected a string");
            throw;
        }
        parser->p++;

        for (;;)
        {
            unsigned char c = (unsigned char)*parser->p;

            if (c == '"')
            {
                parser->p++;
                break;
            }

            if (c == 0)
            {
                json_fail(parser, "unterminated string");
                throw;
            }

            if (c < 0x20)
            {
                json_fail(parser, "unescaped control character in string");
                throw;
            }

            if (c != '\\')
            {
                if (!json_buffer_push(&buffer, (char)c))
                {
                    json_fail(parser, "out of memory");
                    throw;
                }
                parser->p++;
                continue;
            }

            parser->p++; /* the backslash */

            switch (*parser->p)
            {
                case '"':
                    parser->p++;
                    if (!json_buffer_push(&buffer, '"'))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                break;

                case '\\':
                    parser->p++;
                    if (!json_buffer_push(&buffer, '\\'))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                break;

                case '/':
                    parser->p++;
                    if (!json_buffer_push(&buffer, '/'))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                break;

                case 'b':
                    parser->p++;
                    if (!json_buffer_push(&buffer, '\b'))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                break;

                case 'f':
                    parser->p++;
                    if (!json_buffer_push(&buffer, '\f'))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                break;

                case 'n':
                    parser->p++;
                    if (!json_buffer_push(&buffer, '\n'))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                break;

                case 'r':
                    parser->p++;
                    if (!json_buffer_push(&buffer, '\r'))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                break;

                case 't':
                    parser->p++;
                    if (!json_buffer_push(&buffer, '\t'))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                break;

                case 'u':
                {
                    long high;
                    unsigned long codepoint;

                    parser->p++;

                    high = json_read_hex4(parser);
                    if (high < 0)
                    {
                        json_fail(parser, "\\u needs four hex digits");
                        throw;
                    }

                    codepoint = (unsigned long)high;

                    if (high >= 0xD800 && high <= 0xDBFF)
                    {
                        long low;

                        if (parser->p[0] != '\\' || parser->p[1] != 'u')
                        {
                            json_fail(parser, "high surrogate without a low surrogate");
                            throw;
                        }
                        parser->p += 2;

                        low = json_read_hex4(parser);
                        if (low < 0xDC00 || low > 0xDFFF)
                        {
                            json_fail(parser, "invalid low surrogate");
                            throw;
                        }

                        codepoint = 0x10000UL +
                        (((unsigned long)high - 0xD800UL) << 10) +
                        ((unsigned long)low - 0xDC00UL);
                    }
                    else if (high >= 0xDC00 && high <= 0xDFFF)
                    {
                        json_fail(parser, "unpaired low surrogate");
                        throw;
                    }

                    if (!json_buffer_push_codepoint(&buffer, codepoint))
                    {
                        json_fail(parser, "out of memory");
                        throw;
                    }
                    break;
                }

                default:
                    json_fail(parser, "unknown escape sequence");
                    throw;
            }
        }

        /* Reserve before taking the buffer over, so the hand-over itself
           cannot fail and the buffer is never both moved-from and freed on
           one path. */
        if (!json_buffer_reserve(&buffer, 0))
        {
            json_fail(parser, "out of memory");
            throw;
        }

        buffer.data[buffer.length] = 0;
        result = buffer.data;
        buffer.data = NULL;
        buffer.length = 0;
        buffer.capacity = 0;
    }
    catch
    {
    }

    json_buffer_free(&buffer);
    return result;
}

/* Validates the JSON number grammar, then converts with strtod. */
static struct json_value *_Opt _Owner json_parse_number(struct parser *parser)
{
    struct json_value * _Owner _Opt result = NULL;
    const char *begin = parser->p;
    char * _Opt end = NULL;
    double number;

    try
    {
        if (*parser->p == '-')
        {
            parser->p++;
        }

        if (*parser->p == '0')
        {
            parser->p++;
        }
        else if (*parser->p >= '1' && *parser->p <= '9')
        {
            while (*parser->p >= '0' && *parser->p <= '9')
            {
                parser->p++;
            }
        }
        else
        {
            json_fail(parser, "unexpected token");
            throw;
        }

        if (*parser->p == '.')
        {
            parser->p++;
            if (*parser->p < '0' || *parser->p > '9')
            {
                json_fail(parser, "expected a digit after '.'");
                throw;
            }

            while (*parser->p >= '0' && *parser->p <= '9')
            {
                parser->p++;
            }
        }

        if (*parser->p == 'e' || *parser->p == 'E')
        {
            parser->p++;
            if (*parser->p == '+' || *parser->p == '-')
            {
                parser->p++;
            }
            if (*parser->p < '0' || *parser->p > '9')
            {
                json_fail(parser, "expected a digit in the exponent");
                throw;
            }

            while (*parser->p >= '0' && *parser->p <= '9')
            {
                parser->p++;
            }
        }

        number = strtod(begin, &end);
        if (end != parser->p)
        {
            json_fail(parser, "malformed number");
            throw;
        }

        result = calloc(1, sizeof *result);
        if (result == NULL)
        {
            json_fail(parser, "out of memory");
            throw;
        }

        result->type = JSON_NUMBER;
        result->number = number;
    }
    catch
    {
    }

    return result;
}

static struct json_value * _Opt _Owner json_parse_value(struct parser *parser);

static struct json_value * _Opt _Owner json_parse_literal(struct parser *parser,
                                                          const char *text,
                                                          enum json_type type)
{
    struct json_value * _Opt _Owner result = NULL;
    size_t length = strlen(text);

    try
    {
        if (strncmp(parser->p, text, length) != 0)
        {
            json_fail(parser, "unexpected token");
            throw;
        }

        parser->p += length;

        result = calloc(1, sizeof *result);
        if (result == NULL)
        {
            json_fail(parser, "out of memory");
            throw;
        }

        result->type = type;
    }
    catch
    {
    }

    return result;
}

static struct json_value * _Opt _Owner json_parse_array(struct parser *parser)
{
    struct json_value * _Opt _Owner result = NULL;

    try
    {
        result = calloc(1, sizeof *result);
        if (result == NULL)
        {
            json_fail(parser, "out of memory");
            throw;
        }

        result->type = JSON_ARRAY;

        parser->p++; /* '[' */
        json_skip_whitespace(parser);

        if (*parser->p == ']')
        {
            parser->p++;
        }
        else
        {
            for (;;)
            {
                struct json_value * _Opt _Owner item = json_parse_value(parser);
                if (item == NULL)
                {
                    throw;
                }

                json_append_child(result, item);
                json_skip_whitespace(parser);

                if (*parser->p == ',')
                {
                    parser->p++;
                    continue;
                }

                if (*parser->p != ']')
                {
                    json_fail(parser, "expected ',' or ']'");
                    throw;
                }

                parser->p++;
                break;
            }
        }
    }
    catch
    {
        json_delete(result);
        result = NULL;
    }

    return result;
}

static struct json_value * _Opt _Owner  json_parse_object(struct parser *parser)
{
    struct json_value * _Opt _Owner result = NULL;

    try
    {
        result = calloc(1, sizeof *result);
        if (result == NULL)
        {
            json_fail(parser, "out of memory");
            throw;
        }

        result->type = JSON_OBJECT;

        parser->p++; /* '{' */
        json_skip_whitespace(parser);

        if (*parser->p == '}')
        {
            parser->p++;
        }
        else
        {
            for (;;)
            {
                char * _Opt _Owner key = NULL;
                struct json_value * _Opt _Owner member = NULL;

                key = json_parse_string(parser);
                if (key == NULL)
                {
                    throw;
                }

                json_skip_whitespace(parser);

                if (*parser->p != ':')
                {
                    free(key);
                    json_fail(parser, "expected ':' after a member name");
                    throw;
                }
                parser->p++;

                member = json_parse_value(parser);
                if (member == NULL)
                {
                    free(key);
                    throw;
                }

                free(member->key);
                member->key = key;

                json_append_child(result, member);

                json_skip_whitespace(parser);

                if (*parser->p == ',')
                {
                    parser->p++;
                    json_skip_whitespace(parser);
                    continue;
                }

                if (*parser->p != '}')
                {
                    json_fail(parser, "expected ',' or '}'");
                    throw;
                }

                parser->p++;
                break;
            }
        }
    }
    catch
    {
        json_delete(result);
        result = NULL;
    }

    return result;
}

static struct json_value * _Opt _Owner json_parse_value(struct parser *parser)
{
    struct json_value *_Opt _Owner result = NULL;
    char * _Opt _Owner text = NULL;

    json_skip_whitespace(parser);

    try
    {
        if (parser->depth >= JSON_MAX_DEPTH)
        {
            json_fail(parser, "nesting too deep");
            throw;
        }

        switch (*parser->p)
        {
            case '{':
                parser->depth++;
                result = json_parse_object(parser);
                parser->depth--;
            break;

            case '[':
                parser->depth++;
                result = json_parse_array(parser);
                parser->depth--;
            break;

            case '"':
                text = json_parse_string(parser);
                if (text == NULL)
                {
                    throw;
                }

                result = calloc(1, sizeof *result);
                if (result == NULL)
                {
                    json_fail(parser, "out of memory");
                    throw;
                }

                result->type = JSON_STRING;
                result->string = text;
                text = NULL;
            break;

            case 't':
                result = json_parse_literal(parser, "true", JSON_TRUE);
            break;

            case 'f':
                result = json_parse_literal(parser, "false", JSON_FALSE);
            break;

            case 'n':
                result = json_parse_literal(parser, "null", JSON_NULL);
            break;

            case 0:
                json_fail(parser, "unexpected end of input");
            break;

            default:
                result = json_parse_number(parser);
            break;
        }
    }
    catch
    {
    }

    free(text);
    return result;
}

struct json_value * _Opt _Owner json_parse(const char * _Opt text, struct json_error * _Opt error)
{
    struct json_value * _Opt _Owner result = NULL;
    /* Cleared as a whole object rather than just message[0], so the caller's
     struct is fully initialized before parser.error starts pointing at it. */
    const struct json_error cleared = { "", 1, 1 };
    struct parser parser = {0};
    struct json_error local = cleared;

    if (error == NULL)
    {
        error = &local;
    }
    else
    {
        *error = cleared;
    }

    try
    {
        if (text == NULL)
        {
            snprintf(error->message, sizeof error->message, "%s", "no input");
            throw;
        }

        parser.start = text;
        parser.p = text;
        parser.depth = 0;
        parser.error = error;

        result = json_parse_value(&parser);
        if (result == NULL)
        {
            throw;
        }

        json_skip_whitespace(&parser);

        if (*parser.p != 0)
        {
            json_fail(&parser, "trailing content after the document");
            throw;
        }
    }
    catch
    {
        json_delete(result);
        result = NULL;
    }

    return result;
}

void json_delete(struct json_value * _Owner _Opt value)
{
    if (value != NULL)
    {
        struct json_value *_Opt _Owner child = value->first_child;
        value->first_child = NULL;
        while (child != NULL)
        {
            struct json_value * _Opt _Owner next = child->next;
            child->next = NULL;
            json_delete(child);
            child = next;
        }

        free(value->key);
        free(value->string);

        /* value->next belongs to whoever held this node in their list, and is
           always unlinked before the node reaches here - see json_remove_member
           and the loop above. */
        free(value); //lint 29
    }
}

struct json_value * _Opt json_find_member(const struct json_value *object, const char *key)
{
    struct json_value * _Opt result = NULL;

    if (object->type == JSON_OBJECT)
    {
        struct json_value * _Opt child;

        for (child = object->first_child; child != NULL; child = child->next)
        {
            if (child->key != NULL && strcmp(child->key, key) == 0)
            {
                result = child;
                break;
            }
        }
    }

    return result;
}

struct json_value * _Opt json_item(const struct json_value *value, size_t index)
{
    struct json_value * _Opt result = NULL;
    struct json_value * _Opt child;

    for (child = value->first_child; child != NULL; child = child->next)
    {
        if (index == 0)
        {
            result = child;
            break;
        }
        index--;
    }

    return result;
}

size_t json_count(const struct json_value *value)
{
    size_t count = 0;
    struct json_value * _Opt child;

    for (child = value->first_child; child != NULL; child = child->next)
    {
        count++;
    }

    return count;
}

/* Unlinks and frees the member named key, if the object has one. */
static void json_remove_member(struct json_value *object, const char *key)
{
    struct json_value * _Opt previous = NULL;
    struct json_value * _Opt child;

    for (child = object->first_child; child != NULL; child = child->next)
    {
        if (child->key != NULL && strcmp(child->key, key) == 0)
        {
            /* The list links are _Owner, but the walk above is a plain
               observer pointer, so cake cannot see that unlinking `child`
               hands its ownership over to json_delete right below. */
            if (previous != NULL)
            {
                previous->next = child->next; //lint 26
            }
            else
            {
                object->first_child = child->next; //lint 26
            }

            if (object->last_child == child)
            {
                object->last_child = previous;
            }

            child->next = NULL;
            json_delete(child); //lint 78
            break;
        }
        previous = child;
    }
}

struct json_value * _Opt json_set_string(struct json_value *object, const char *key, const char *text)
{
    struct json_value * _Opt result = NULL;
    char * _Opt _Owner key_copy = NULL;
    char * _Opt _Owner copy = NULL;

    try
    {
        copy = strdup(text);
        if (copy == NULL)
        {
            throw;
        }

        key_copy = strdup(key);
        if (key_copy == NULL)
        {
            throw;
        }

        struct json_value * _Opt _Owner value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        /* Only now that nothing else can fail: replacing the old member of
           the same name must not happen on a path that then gives up. */
        json_remove_member(object, key);

        value->type = JSON_STRING;
        value->key = key_copy;
        key_copy = NULL;
        value->string = copy;
        copy = NULL;

        json_append_child(object, value);
        result = object->last_child;
    }
    catch
    {
    }

    free(key_copy);
    free(copy);
    return result;
}

struct json_value * _Opt json_set_number(struct json_value *object, const char *key, double number)
{
    struct json_value * _Opt result = NULL;
    char * _Opt _Owner key_copy = NULL;

    try
    {
        key_copy = strdup(key);
        if (key_copy == NULL)
        {
            throw;
        }

        struct json_value * _Opt _Owner value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        /* Only now that nothing else can fail: replacing the old member of
           the same name must not happen on a path that then gives up. */
        json_remove_member(object, key);

        value->type = JSON_NUMBER;
        value->key = key_copy;
        key_copy = NULL;
        value->number = number;

        json_append_child(object, value);
        result = object->last_child;
    }
    catch
    {
    }

    free(key_copy);
    return result;
}

struct json_value * _Opt json_set_bool(struct json_value *object, const char *key, int boolean_value)
{
    struct json_value * _Opt result = NULL;
    char * _Opt _Owner key_copy = NULL;

    try
    {
        key_copy = strdup(key);
        if (key_copy == NULL)
        {
            throw;
        }

        struct json_value * _Opt _Owner value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        /* Only now that nothing else can fail: replacing the old member of
           the same name must not happen on a path that then gives up. */
        json_remove_member(object, key);

        value->type = boolean_value ? JSON_TRUE : JSON_FALSE;
        value->key = key_copy;
        key_copy = NULL;

        json_append_child(object, value);
        result = object->last_child;
    }
    catch
    {
    }

    free(key_copy);
    return result;
}

struct json_value * _Opt json_set_null(struct json_value *object, const char *key)
{
    struct json_value * _Opt result = NULL;
    char * _Opt _Owner key_copy = NULL;

    try
    {
        key_copy = strdup(key);
        if (key_copy == NULL)
        {
            throw;
        }

        struct json_value * _Opt _Owner value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        /* Only now that nothing else can fail: replacing the old member of
           the same name must not happen on a path that then gives up. */
        json_remove_member(object, key);

        value->type = JSON_NULL;
        value->key = key_copy;
        key_copy = NULL;

        json_append_child(object, value);
        result = object->last_child;
    }
    catch
    {
    }

    free(key_copy);
    return result;
}

struct json_value * _Opt json_set_object(struct json_value *object, const char *key)
{
    struct json_value * _Opt result = NULL;
    char * _Opt _Owner key_copy = NULL;

    try
    {
        key_copy = strdup(key);
        if (key_copy == NULL)
        {
            throw;
        }

        struct json_value * _Opt _Owner value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        /* Only now that nothing else can fail: replacing the old member of
           the same name must not happen on a path that then gives up. */
        json_remove_member(object, key);

        value->type = JSON_OBJECT;
        value->key = key_copy;
        key_copy = NULL;

        json_append_child(object, value);
        result = object->last_child;
    }
    catch
    {
    }

    free(key_copy);
    return result;
}

struct json_value * _Opt json_set_array(struct json_value *object, const char *key)
{
    struct json_value * _Opt result = NULL;
    char * _Opt _Owner key_copy = NULL;

    try
    {
        key_copy = strdup(key);
        if (key_copy == NULL)
        {
            throw;
        }

        struct json_value * _Opt _Owner value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        /* Only now that nothing else can fail: replacing the old member of
           the same name must not happen on a path that then gives up. */
        json_remove_member(object, key);

        value->type = JSON_ARRAY;
        value->key = key_copy;
        key_copy = NULL;

        json_append_child(object, value);
        result = object->last_child;
    }
    catch
    {
    }

    free(key_copy);
    return result;
}

struct json_value * _Opt json_add_string(struct json_value *array, const char *text)
{
    struct json_value * _Opt result = NULL;
    char * _Opt _Owner copy = NULL;

    try
    {
        copy = strdup(text);
        if (copy == NULL)
        {
            throw;
        }

        struct json_value * _Owner _Opt value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        value->type = JSON_STRING;
        value->string = copy;
        copy = NULL;

        json_append_child(array, value);
        result = array->last_child;
    }
    catch
    {
    }

    free(copy);
    return result;
}

struct json_value * _Opt json_add_number(struct json_value *array, double number)
{
    struct json_value * _Opt result = NULL;

    try
    {
        struct json_value * _Owner _Opt value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        value->type = JSON_NUMBER;
        value->number = number;

        json_append_child(array, value);
        result = array->last_child;
    }
    catch
    {
    }

    return result;
}

struct json_value * _Opt json_add_bool(struct json_value *array, int boolean_value)
{
    struct json_value * _Opt result = NULL;

    try
    {
        struct json_value * _Owner _Opt value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        value->type = boolean_value ? JSON_TRUE : JSON_FALSE;

        json_append_child(array, value);
        result = array->last_child;
    }
    catch
    {
    }

    return result;
}

struct json_value * _Opt json_add_null(struct json_value *array)
{
    struct json_value * _Opt result = NULL;

    try
    {
        struct json_value * _Owner _Opt value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        value->type = JSON_NULL;

        json_append_child(array, value);
        result = array->last_child;
    }
    catch
    {
    }

    return result;
}

struct json_value * _Opt json_add_object(struct json_value *array)
{
    struct json_value * _Opt result = NULL;

    try
    {
        struct json_value * _Owner _Opt value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        value->type = JSON_OBJECT;

        json_append_child(array, value);
        result = array->last_child;
    }
    catch
    {
    }

    return result;
}

struct json_value * _Opt json_add_array(struct json_value *array)
{
    struct json_value * _Opt result = NULL;

    try
    {
        struct json_value * _Owner _Opt value = calloc(1, sizeof *value);
        if (value == NULL)
        {
            throw;
        }

        value->type = JSON_ARRAY;

        json_append_child(array, value);
        result = array->last_child;
    }
    catch
    {
    }

    return result;
}

static void json_write_quoted(FILE *file, const char *text)
{
    const unsigned char *p = (const unsigned char *)text;

    putc('"', file);

    for (; *p != 0; p++)
    {
        switch (*p)
        {
            case '"':  fputs("\\\"", file); break;
            case '\\': fputs("\\\\", file); break;
            case '\b': fputs("\\b", file); break;
            case '\f': fputs("\\f", file); break;
            case '\n': fputs("\\n", file); break;
            case '\r': fputs("\\r", file); break;
            case '\t': fputs("\\t", file); break;
            default:
                if (*p < 0x20)
                {
                    fprintf(file, "\\u%04x", *p);
                }
                else
                {
                    putc((char)*p, file);
                }
            break;
        }
    }

    putc('"', file);
}

static void json_write_number(FILE *file, double number)
{
    if (number == (double)(long)number && number > -1e9 && number < 1e9)
    {
        fprintf(file, "%ld", (long)number);
    }
    else
    {
        fprintf(file, "%.17g", number);
    }
}

static void json_write_indent(FILE *file, int depth)
{
    int i;
    for (i = 0; i < depth; i++)
    {
        fputs("  ", file);
    }
}

static void json_write_value(FILE *file, const struct json_value *value, int depth)
{
    switch (value->type)
    {
        case JSON_OBJECT:
        case JSON_ARRAY:
        {
            int is_object = value->type == JSON_OBJECT;

            if (value->first_child == NULL)
            {
                fputs(is_object ? "{}" : "[]", file);
            }
            else
            {
                const struct json_value * _Opt child;

                fputs(is_object ? "{\n" : "[\n", file);

                for (child = value->first_child; child != NULL; child = child->next)
                {
                    json_write_indent(file, depth + 1);

                    if (is_object && child->key != NULL)
                    {
                        json_write_quoted(file, child->key);
                        fputs(": ", file);
                    }

                    json_write_value(file, child, depth + 1);

                    if (child->next != NULL)
                    {
                        putc(',', file);
                    }
                    putc('\n', file);
                }

                json_write_indent(file, depth);
                putc(is_object ? '}' : ']', file);
            }
        }
        break;

        case JSON_STRING:
            json_write_quoted(file, value->string != NULL ? value->string : "");
        break;

        case JSON_NUMBER:
            json_write_number(file, value->number);
        break;

        case JSON_TRUE:
            fputs("true", file);
        break;

        case JSON_FALSE:
            fputs("false", file);
        break;

        case JSON_NULL:
            fputs("null", file);
        break;
    }
}

int json_write_file(const char *path, const struct json_value *root)
{
    int result = 0;
    FILE * _Opt _Owner file = NULL;

    try
    {
        int failed;

        file = fopen(path, "w");
        if (file == NULL)
        {
            throw;
        }

        json_write_value(file, root, 0);
        putc('\n', file);

        failed = ferror(file);

        if (fclose(file) != 0)
        {
            failed = 1;
        }

        result = failed ? 0 : 1;
    }
    catch
    {
    }

    return result;
}
