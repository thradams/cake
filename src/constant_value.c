#pragma safety enable

#include "ownership.h"
#include "constant_value.h"
#include <limits.h>
#include <stdio.h>
#include <assert.h>

bool unsigned_long_long_sub(_Out unsigned long long* result, unsigned long long a, unsigned long long b)
{
    if (a < b)
        return false;

    *result = a - b;
    return true;
}

bool unsigned_long_long_mul(_Out unsigned long long* result, unsigned long long a, unsigned long long b)
{

    if (b == 0)
    {
        /*
          b cannot be zero in the next test
          so we solve this case here
        */
        *result = 0;
        return true;
    }

    if (a > ULLONG_MAX / b)
        return false;

    *result = a * b;
    return true;
}

bool unsigned_long_long_add(_Out unsigned long long* result, unsigned long long a, unsigned long long b)
{
    if (a > ULLONG_MAX - b)
    {
        //a=2
        //b=254
        return false;
    }
    *result = a + b;
    return true;
}

bool signed_long_long_sub(_Out signed long long* result, signed long long a, signed long long b)
{
    if (a >= 0 && b >= 0)
    {
    }
    else if (a < 0 && b < 0)
    {
    }
    else
    {
        if (a < 0)
        {
            if (a < LLONG_MIN + b)
            {
                // (-128) - (-1)
                return false;
            }
        }
        else
        {
            if (b == LLONG_MIN)
            {
                // 0 - (-128)                
                return false;
            }

            if (a > LLONG_MAX - (-b))
            {
                /*
                *  1 - (-127)
                *  2 - (-126)
                */
                return false;
            }
        }
    }

    *result = a - b;
    return true;
}

bool signed_long_long_add(_Out signed long long* result, signed long long a, signed long long b)
{

    if (a >= 0 && b >= 0)
    {
        /*both positive*/
        if (a > LLONG_MAX - b)
        {
            //2+126
            return false;
        }
    }
    else if (a < 0 && b < 0)
    {

        if (a == LLONG_MIN || b == LLONG_MIN)
        {
            //(-128) + (-128)
            return false;
        }

        if (a < LLONG_MIN - b)
        {
            // (-127) + (-127)
            return false;
        }
    }
    else
    {
        /*one positive another negative*/

    }

    *result = a + b;
    return true;
}

bool signed_long_long_mul(_Out signed long long* result, signed long long a, signed long long b)
{

    if (a > 0 && b > 0)
    {
        if (a > LLONG_MAX / b)
        {
            //2*64
            return false;
        }
    }
    else if (a < 0 && b < 0)
    {

        if (a == LLONG_MIN || b == LLONG_MIN)
        {
            //(-128)*(-128)
            return false;
        }

        if (-a > LLONG_MAX / -b)
        {
            //-127 * -127
            return false;
        }
    }
    else
    {
        if (a == 0 || b == 0)
        {
            *result = 0;
            return true;
        }
        if (b > 0)
        {
            if (a < LLONG_MIN / b)
                //(-127) * (2)
                return false;
        }
        else
        {
            if (b < LLONG_MIN / a)
            {
                //2*(-128)
                return false;
            }
        }
    }

    *result = a * b;
    return true;
}

bool constant_value_is_valid(const struct constant_value* a)
{
    return a->type != TYPE_NOT_CONSTANT;
}

void constant_value_to_string(const struct constant_value* a, char buffer[], int sz)
{
    buffer[0] = 0;
    switch (a->type)
    {

    case TYPE_NOT_CONSTANT:
        snprintf(buffer, sz, "(not-const)");
        break;

    case TYPE_BOOL:
        snprintf(buffer, sz, "%s", a->signed_char_value ? "true" : "false");
        break;

    case TYPE_SIGNED_CHAR:
        snprintf(buffer, sz, "%c", a->signed_char_value);
        break;

    case TYPE_UNSIGNED_CHAR:
        snprintf(buffer, sz, "%c", a->unsigned_char_value);
        break;

    case TYPE_SIGNED_SHORT:
        snprintf(buffer, sz, "%c", a->signed_short_value);
        break;
    case TYPE_UNSIGNED_SHORT:
        snprintf(buffer, sz, "%c", a->signed_short_value);
        break;

    case TYPE_SIGNED_INT:
        snprintf(buffer, sz, "%d", a->signed_int_value);
        break;

    case TYPE_UNSIGNED_INT:
        snprintf(buffer, sz, "%u", a->signed_int_value);
        break;

    case TYPE_SIGNED_LONG:
        snprintf(buffer, sz, "%ld", a->signed_long_value);
        break;

    case TYPE_UNSIGNED_LONG:
        break;

    case TYPE_SIGNED_LONG_LONG:
        snprintf(buffer, sz, "%lud", a->signed_long_value);
        break;

    case TYPE_UNSIGNED_LONG_LONG:
        snprintf(buffer, sz, "%llu", a->signed_long_long_value);
        break;

    case TYPE_FLOAT:
        snprintf(buffer, sz, "%f", a->float_value);
        break;

    case TYPE_DOUBLE:
        snprintf(buffer, sz, "%f", a->double_value);
        break;

    case TYPE_LONG_DOUBLE:
        snprintf(buffer, sz, "%Lf", a->long_double_value);
        break;
    }
}

struct constant_value constant_value_make_size_t(size_t value)
{
    struct constant_value r = { 0 };

#if defined(_WIN64) || defined(__x86_64__) 
    r.type = TYPE_UNSIGNED_LONG_LONG;
    r.unsigned_long_long_value = value;
#else
    r.type = TYPE_UNSIGNED_INT;
    r.unsigned_int_value = value;
#endif

    return r;
}

struct constant_value constant_value_make_nullptr()
{
    struct constant_value r = { 0 };
    r.type = TYPE_SIGNED_INT;
    r.signed_short_value = 0;
    return r;
}

struct constant_value constant_value_make_wchar_t(wchar_t value)
{
    struct constant_value r = { 0 };
#ifdef _WIN32
    static_assert(_Generic(L' ', unsigned short : 1), "");
    r.type = TYPE_SIGNED_SHORT;
    r.signed_short_value = value;
#else
    static_assert(_Generic(L' ', int : 1), "");
    r.type = TYPE_SIGNED_INT;
    r.signed_int_value = value;
#endif

    return r;
}

struct constant_value constant_value_make_bool(bool value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_BOOL;
    r.bool_value = value;
    return r;
}

#pragma warning( push )
#pragma warning( disable : 4244 )

bool constant_value_to_bool(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_signed_char(signed char value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_SIGNED_CHAR;
    r.signed_char_value = value;
    return r;
}

signed char constant_value_to_signed_char(const struct constant_value* a)
{


    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}

struct constant_value constant_value_make_unsigned_char(unsigned char value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_UNSIGNED_CHAR;
    r.unsigned_char_value = value;
    return r;
}

unsigned char constant_value_to_unsigned_char(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_signed_short(signed short value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_SIGNED_SHORT;
    r.signed_short_value = value;
    return r;
}

signed short constant_value_to_signed_short(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_unsigned_short(unsigned short value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_UNSIGNED_SHORT;
    r.unsigned_short_value = value;
    return r;
}

unsigned short constant_value_to_unsigned_short(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_signed_int(signed int value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_SIGNED_INT;
    r.signed_int_value = value;
    return r;
}

signed int constant_value_to_signed_int(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_unsigned_int(unsigned int value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_UNSIGNED_INT;
    r.unsigned_int_value = value;
    return r;
}

unsigned int constant_value_to_unsigned_int(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_signed_long(signed long value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_SIGNED_LONG;
    r.signed_long_value = value;
    return r;
}

signed long constant_value_to_signed_long(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_unsigned_long(unsigned long value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_UNSIGNED_LONG;
    r.unsigned_long_value = value;
    return r;
}

unsigned long constant_value_to_unsigned_long(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_signed_long_long(signed long long value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_SIGNED_LONG_LONG;
    r.signed_long_long_value = value;
    return r;
}

signed long long constant_value_to_signed_long_long(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_unsigned_long_long(unsigned long long value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_UNSIGNED_LONG_LONG;
    r.unsigned_long_long_value = value;
    return r;
}

unsigned long long constant_value_to_unsigned_long_long(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_float(float value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_FLOAT;
    r.float_value = value;
    return r;
}

float constant_value_to_float(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_double(double value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_DOUBLE;
    r.double_value = value;
    return r;
}

double constant_value_to_double(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}
struct constant_value constant_value_make_long_double(long double value)
{
    struct constant_value r = { 0 };
    r.type = TYPE_LONG_DOUBLE;
    r.long_double_value = value;
    return r;
}

long double constant_value_to_long_double(const struct constant_value* a)
{
    switch (a->type)
    {
    case TYPE_NOT_CONSTANT:break;
    case TYPE_BOOL: return a->bool_value;
    case TYPE_SIGNED_CHAR: return a->signed_char_value;
    case TYPE_UNSIGNED_CHAR: return a->unsigned_char_value;
    case TYPE_SIGNED_SHORT: return a->signed_short_value;
    case TYPE_UNSIGNED_SHORT: return a->unsigned_short_value;
    case TYPE_SIGNED_INT: return a->signed_int_value;
    case TYPE_UNSIGNED_INT: return a->unsigned_int_value;
    case TYPE_SIGNED_LONG: return a->signed_long_value;
    case TYPE_UNSIGNED_LONG: return a->unsigned_long_value;
    case TYPE_SIGNED_LONG_LONG: return a->signed_long_long_value;
    case TYPE_UNSIGNED_LONG_LONG: return a->unsigned_long_long_value;
    case TYPE_FLOAT: return a->float_value;
    case TYPE_DOUBLE: return a->double_value;
    case TYPE_LONG_DOUBLE: return a->long_double_value;
    }
    assert(0);
    return 0;
}



#pragma warning( pop )



struct constant_value constant_value_cast(enum constant_value_type t, const struct constant_value* v)
{
    //No changes
    if (v->type == t)
        return *v;


    //This function is generated by this program
    /*
            struct type
            {
                const char * type;
                const char * name;
                const char * value_type;
            };
            struct type types[] =
            {
                {"bool", "bool", "TYPE_BOOL"},
                {"signed char", "signed_char", "TYPE_SIGNED_CHAR"},
                {"unsigned char", "unsigned_char", "TYPE_UNSIGNED_CHAR"},
                {"signed short", "signed_short", "TYPE_SIGNED_SHORT"},
                {"unsigned short", "unsigned_short", "TYPE_UNSIGNED_SHORT"},
                {"signed int", "signed_int", "TYPE_SIGNED_INT"},
                {"unsigned int", "unsigned_int", "TYPE_UNSIGNED_INT"},
                {"signed long", "signed_long", "TYPE_SIGNED_LONG"},
                {"unsigned long", "unsigned_long", "TYPE_UNSIGNED_LONG"},
                {"signed long long", "signed_long_long", "TYPE_SIGNED_LONG_LONG"},
                {"unsigned long long", "unsigned_long_long", "TYPE_UNSIGNED_LONG_LONG"},
                {"float", "float", "TYPE_FLOAT"},
                {"double", "double", "TYPE_DOUBLE"},
                {"long double", "long_double", "TYPE_LONG_DOUBLE"}
            };


            int main()
            {
                FILE * f = fopen("imp.c", "w");
                if (f == NULL)
                    return;

                fprintf(f, "struct constant_value cast(enum constant_value_type t, struct constant_value * v)\n");
                fprintf(f, "{\n");
                for (int i = 0; i < sizeof(types) / sizeof(types[0]); i++)
                {
                    fprintf(f, "if (t == %s)\n", types[i].value_type);
                    fprintf(f, "{\n");
                    for (int j = 0; j < sizeof(types) / sizeof(types[0]); j++)
                    {
                        if (i == j)
                            continue;
                        fprintf(f, "if (v->type == %s)\n", types[j].value_type);
                        fprintf(f, " return constant_value_make_%s((%s)v->%s_value);\n", types[i].name, types[i].type, types[j].name);
                    }
                    fprintf(f, "}\n");
                }
                fprintf(f, "}\n");

                fclose(f);
            }
    */
    if (t == TYPE_BOOL)
    {
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_bool((bool)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_bool((bool)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_bool((bool)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_bool((bool)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_bool((bool)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_bool((bool)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_bool((bool)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_bool((bool)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_bool((bool)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_bool((bool)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_bool((bool)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_bool((bool)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_bool((bool)v->long_double_value);
    }
    if (t == TYPE_SIGNED_CHAR)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_signed_char((signed char)v->bool_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_signed_char((signed char)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_signed_char((signed char)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_signed_char((signed char)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_signed_char((signed char)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_signed_char((signed char)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_signed_char((signed char)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_signed_char((signed char)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_signed_char((signed char)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_signed_char((signed char)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_signed_char((signed char)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_signed_char((signed char)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_signed_char((signed char)v->long_double_value);
    }
    if (t == TYPE_UNSIGNED_CHAR)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_unsigned_char((unsigned char)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_unsigned_char((unsigned char)v->signed_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_unsigned_char((unsigned char)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_unsigned_char((unsigned char)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_unsigned_char((unsigned char)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_unsigned_char((unsigned char)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_unsigned_char((unsigned char)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_unsigned_char((unsigned char)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_unsigned_char((unsigned char)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_unsigned_char((unsigned char)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_unsigned_char((unsigned char)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_unsigned_char((unsigned char)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_unsigned_char((unsigned char)v->long_double_value);
    }
    if (t == TYPE_SIGNED_SHORT)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_signed_short((signed short)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_signed_short((signed short)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_signed_short((signed short)v->unsigned_char_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_signed_short((signed short)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_signed_short((signed short)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_signed_short((signed short)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_signed_short((signed short)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_signed_short((signed short)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_signed_short((signed short)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_signed_short((signed short)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_signed_short((signed short)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_signed_short((signed short)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_signed_short((signed short)v->long_double_value);
    }
    if (t == TYPE_UNSIGNED_SHORT)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_unsigned_short((unsigned short)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_unsigned_short((unsigned short)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_unsigned_short((unsigned short)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_unsigned_short((unsigned short)v->signed_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_unsigned_short((unsigned short)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_unsigned_short((unsigned short)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_unsigned_short((unsigned short)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_unsigned_short((unsigned short)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_unsigned_short((unsigned short)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_unsigned_short((unsigned short)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_unsigned_short((unsigned short)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_unsigned_short((unsigned short)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_unsigned_short((unsigned short)v->long_double_value);
    }
    if (t == TYPE_SIGNED_INT)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_signed_int((signed int)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_signed_int((signed int)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_signed_int((signed int)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_signed_int((signed int)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_signed_int((signed int)v->unsigned_short_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_signed_int((signed int)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_signed_int((signed int)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_signed_int((signed int)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_signed_int((signed int)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_signed_int((signed int)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_signed_int((signed int)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_signed_int((signed int)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_signed_int((signed int)v->long_double_value);
    }
    if (t == TYPE_UNSIGNED_INT)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_unsigned_int((unsigned int)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_unsigned_int((unsigned int)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_unsigned_int((unsigned int)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_unsigned_int((unsigned int)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_unsigned_int((unsigned int)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_unsigned_int((unsigned int)v->signed_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_unsigned_int((unsigned int)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_unsigned_int((unsigned int)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_unsigned_int((unsigned int)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_unsigned_int((unsigned int)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_unsigned_int((unsigned int)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_unsigned_int((unsigned int)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_unsigned_int((unsigned int)v->long_double_value);
    }
    if (t == TYPE_SIGNED_LONG)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_signed_long((signed long)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_signed_long((signed long)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_signed_long((signed long)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_signed_long((signed long)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_signed_long((signed long)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_signed_long((signed long)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_signed_long((signed long)v->unsigned_int_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_signed_long((signed long)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_signed_long((signed long)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_signed_long((signed long)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_signed_long((signed long)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_signed_long((signed long)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_signed_long((signed long)v->long_double_value);
    }
    if (t == TYPE_UNSIGNED_LONG)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_unsigned_long((unsigned long)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_unsigned_long((unsigned long)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_unsigned_long((unsigned long)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_unsigned_long((unsigned long)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_unsigned_long((unsigned long)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_unsigned_long((unsigned long)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_unsigned_long((unsigned long)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_unsigned_long((unsigned long)v->signed_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_unsigned_long((unsigned long)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_unsigned_long((unsigned long)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_unsigned_long((unsigned long)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_unsigned_long((unsigned long)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_unsigned_long((unsigned long)v->long_double_value);
    }
    if (t == TYPE_SIGNED_LONG_LONG)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_signed_long_long((signed long long)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_signed_long_long((signed long long)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_signed_long_long((signed long long)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_signed_long_long((signed long long)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_signed_long_long((signed long long)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_signed_long_long((signed long long)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_signed_long_long((signed long long)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_signed_long_long((signed long long)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_signed_long_long((signed long long)v->unsigned_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_signed_long_long((signed long long)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_signed_long_long((signed long long)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_signed_long_long((signed long long)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_signed_long_long((signed long long)v->long_double_value);
    }
    if (t == TYPE_UNSIGNED_LONG_LONG)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_unsigned_long_long((unsigned long long)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_unsigned_long_long((unsigned long long)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_unsigned_long_long((unsigned long long)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_unsigned_long_long((unsigned long long)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_unsigned_long_long((unsigned long long)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_unsigned_long_long((unsigned long long)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_unsigned_long_long((unsigned long long)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_unsigned_long_long((unsigned long long)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_unsigned_long_long((unsigned long long)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_unsigned_long_long((unsigned long long)v->signed_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_unsigned_long_long((unsigned long long)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_unsigned_long_long((unsigned long long)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_unsigned_long_long((unsigned long long)v->long_double_value);
    }
    if (t == TYPE_FLOAT)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_float((float)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_float((float)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_float((float)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_float((float)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_float((float)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_float((float)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_float((float)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_float((float)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_float((float)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_float((float)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_float((float)v->unsigned_long_long_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_float((float)v->double_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_float((float)v->long_double_value);
    }
    if (t == TYPE_DOUBLE)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_double((double)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_double((double)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_double((double)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_double((double)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_double((double)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_double((double)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_double((double)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_double((double)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_double((double)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_double((double)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_double((double)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_double((double)v->float_value);
        if (v->type == TYPE_LONG_DOUBLE)
            return constant_value_make_double((double)v->long_double_value);
    }
    if (t == TYPE_LONG_DOUBLE)
    {
        if (v->type == TYPE_BOOL)
            return constant_value_make_long_double((long double)v->bool_value);
        if (v->type == TYPE_SIGNED_CHAR)
            return constant_value_make_long_double((long double)v->signed_char_value);
        if (v->type == TYPE_UNSIGNED_CHAR)
            return constant_value_make_long_double((long double)v->unsigned_char_value);
        if (v->type == TYPE_SIGNED_SHORT)
            return constant_value_make_long_double((long double)v->signed_short_value);
        if (v->type == TYPE_UNSIGNED_SHORT)
            return constant_value_make_long_double((long double)v->unsigned_short_value);
        if (v->type == TYPE_SIGNED_INT)
            return constant_value_make_long_double((long double)v->signed_int_value);
        if (v->type == TYPE_UNSIGNED_INT)
            return constant_value_make_long_double((long double)v->unsigned_int_value);
        if (v->type == TYPE_SIGNED_LONG)
            return constant_value_make_long_double((long double)v->signed_long_value);
        if (v->type == TYPE_UNSIGNED_LONG)
            return constant_value_make_long_double((long double)v->unsigned_long_value);
        if (v->type == TYPE_SIGNED_LONG_LONG)
            return constant_value_make_long_double((long double)v->signed_long_long_value);
        if (v->type == TYPE_UNSIGNED_LONG_LONG)
            return constant_value_make_long_double((long double)v->unsigned_long_long_value);
        if (v->type == TYPE_FLOAT)
            return constant_value_make_long_double((long double)v->float_value);
        if (v->type == TYPE_DOUBLE)
            return constant_value_make_long_double((long double)v->double_value);
    }
    struct constant_value empty = { 0 };
    return empty;
}

