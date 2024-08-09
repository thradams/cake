#if 0

//GENERATES unit test
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))

#define LONGSIZE sizeof(long)

struct lit {
    const char * value;
    const char * type;
    int size;
};
struct lit literals[] =
{    
    {"1","int", sizeof(int)},
{ "1L", "long" , LONGSIZE},
{ "1LL", "long long" , sizeof(long long)},
{ "1U" , "unsigned" , sizeof(unsigned)},
{"1UL", "unsigned long", LONGSIZE},
{"1ULL", "unsigned long long", sizeof(unsigned long long)},
{"1.0", "double", sizeof(double)},
{"1.0f", "float", sizeof(float)},
{"1.0L", "long double", sizeof(long double)},
};

const char * comparision_operators[] = { "==", "!=" };
const char * arithemtic_operators[] = { "+", "-", "*", "/", "%" };
const char * relational_operators[] = { ">", ">=", "<", "<=" };
const char * bitwise_operators[] = { "&", "|", "^", "<<", ">>", "~" };

int is_signed2(const char * type)
{
    return strcmp(type, "int") == 0 ||
        strcmp(type, "long") == 0 ||
        strcmp(type, "long long") == 0;        

}

const char * return_unsigned(const char * type)
{
    if (strcmp(type, "int") == 0) return "unsigned";
    if (strcmp(type, "long") == 0) return "unsigned long";

    if (strcmp(type, "long long") == 0) return "unsigned long long";

    return "???";
}


int is_unsigned2(const char * type)
{
    return strcmp(type, "unsigned") == 0 ||
        strcmp(type, "unsigned long") == 0 ||
        strcmp(type, "unsigned long long") == 0;
}

int is_float2(const char * type)
{
    return strcmp(type, "float") == 0 ||
        strcmp(type, "double") == 0 ||
        strcmp(type, "long double") == 0;
}

int rank(const struct lit * a)
{
    if (strcmp(a->type, "unsigned long long") == 0)
        return 10;
    if (strcmp(a->type, "long long") == 0)
        return 10;

    if (strcmp(a->type, "unsigned long") == 0)
        return 9;
    if (strcmp(a->type, "long") == 0)
        return 9;

    if (strcmp(a->type, "unsigned") == 0)
        return 8;
    if (strcmp(a->type, "int") == 0)
        return 8;

    return 1;
}

const char * printtype(const struct lit * a, const struct lit * b)
{
    if (strcmp("1.0L", a->value) == 0 || strcmp("1.0L", b->value) == 0)
    {
        //First, if either operand is long double, the other is converted to long double
        return "long double";
    }

    if (strcmp("1.0", a->value) == 0 || strcmp("1.0", b->value) == 0)
    {
        //Otherwise, if either operand is double, the other is converted to double
        return "double";
    }

    if (strcmp("1.0f", a->value) == 0 || strcmp("1.0f", b->value) == 0)
    {
        //Otherwise, if either operand is float, the other is converted to float
        return "float";
    }

    if (strcmp("1ULL", a->value) == 0 || strcmp("1ULL", b->value) == 0)
    {
        return "unsigned long long";
    }


    if (strcmp("1LL", a->value) == 0)
    {
        if (strcmp("1LL", b->value) == 0)
            return "long long";

        if (strcmp("1UL", b->value) == 0)
            return "unsigned long long";

        if (strcmp("1L", b->value) == 0)
            return "long long";

        if (strcmp("1U", b->value) == 0)
            return "long long";

        if (strcmp("1", b->value) == 0)
            return "long long";
    }

    if (strcmp("1UL", a->value) == 0)
    {
        if (strcmp("1LL", b->value) == 0)
            return "unsigned long long";

        if (strcmp("1UL", b->value) == 0)
            return "unsigned long";

        if (strcmp("1L", b->value) == 0)
            return "unsigned long";

        if (strcmp("1U", b->value) == 0)
            return "unsigned long";

        if (strcmp("1", b->value) == 0)
            return "unsigned long";
    }

    if (strcmp("1L", a->value) == 0)
    {
        if (strcmp("1LL", b->value) == 0)
            return  "long long";

        if (strcmp("1UL", b->value) == 0)
            return  "unsigned long";

        if (strcmp("1L", b->value) == 0)
            return  "long";

        if (strcmp("1U", b->value) == 0)
            return  "long";

        if (strcmp("1", b->value) == 0)
            return  "long";
    }

    if (strcmp("1U", a->value) == 0)
    {
        if (strcmp("1LL", b->value) == 0)
            return  "long long";

        if (strcmp("1UL", b->value) == 0)
            return  "unsigned long";

        if (strcmp("1L", b->value) == 0)
            return  "long";

        if (strcmp("1U", b->value) == 0)
            return  "unsigned";

        if (strcmp("1", b->value) == 0)
            return  "unsigned";
    }

    if (strcmp("1", a->value) == 0)
    {
        if (strcmp("1LL", b->value) == 0)
            return  "long long";

        if (strcmp("1UL", b->value) == 0)
            return  "unsigned long";

        if (strcmp("1L", b->value) == 0)
            return  "long";

        if (strcmp("1U", b->value) == 0)
            return  "unsigned";
    }




    return "int";
}

const char * printtype2(const struct lit * a, const struct lit * b)
{
    if (strcmp("1.0L", a->value) == 0 || strcmp("1.0L", b->value) == 0)
    {
        //First, if either operand is long double, the other is converted to long double
        return "long double";
    }

    if (strcmp("1.0", a->value) == 0 || strcmp("1.0", b->value) == 0)
    {
        //Otherwise, if either operand is double, the other is converted to double
        return "double";
    }

    if (strcmp("1.0f", a->value) == 0 || strcmp("1.0f", b->value) == 0)
    {
        //Otherwise, if either operand is float, the other is converted to float
        return "float";
    }


    //if both operands have the same type, then no further conversion is needed.
    if (strcmp(a->type, b->type) == 0)
    {

        return a->type;
    }

    if (is_signed2(a->type) == is_signed2(b->type))
    {
        /*
          Otherwise, if both operands have signed integer types or both have unsigned integer
          types, the operand with the type of lesser integer conversion rank is converted to the type
          of the operand with greater rank
        */
        if (rank(a) > rank(b))
        {
            /*
                long x int
                long long x int
                long long x long
                unsigned long x unsigned
                unsigned long long x unsigned
                unsigned long long x unsigned long

            */
            return a->type;
        }
        else
        {
            /*
             int           x long
             int           x long long
             long          x long long
             unsigned      x unsigned long
             unsigned      x unsigned long long
             unsigned long x unsigned long long
            */
            return b->type;
        }
    }

    const struct lit * tsigned = is_signed2(a->type) ? a : b;
    const struct lit * tunsigned = is_unsigned2(a->type) ? a : b;

    assert(tsigned != tunsigned);
    assert(is_signed2(tsigned->type));
    assert(is_unsigned2(tunsigned->type));
    /*
    * Otherwise, if the operand that has unsigned integer type has rank greater or equal to
    the rank of the type of the other operand, then the operand with signed integer type is
    converted to the type of the operand with unsigned integer type
    */
    if (rank(tunsigned) >= rank(tsigned))
    {
        return tunsigned->type;
    }


    /*
     Otherwise, if the type of the operand with signed integer type can represent all the values
     of the type of the operand with unsigned integer type, then the operand with unsigned
     integer type is converted to the type of the operand with signed integer type
    */

    if (tsigned->size > tunsigned->size)
    {
        //1UL + 1LL -> unsigned long long
        return tsigned->type;
    }

    /*Otherwise, both operands are converted to the unsigned integer type corresponding to
     the type of the operand with signed integer type
    */

    return return_unsigned(tsigned->type);

}

int main()
{
    /*
0        {"1","int", sizeof(int)},
1{ "1L", "long" , LONGSIZE},
2 { "1LL", "long long" , sizeof(long long)},
3 { "1U" , "unsigned" , sizeof(unsigned)},
4 {"1UL", "unsigned long", LONGSIZE},
5 {"1ULL", "unsigned long long", sizeof(unsigned long long)},

    */
    //static_assert(TYPE_OF(1LL + 1UL, long long));
    //static_assert(TYPE_OF(1L + 1U, unsigned long));
    //1LL + 1UL
    //static_assert(TYPE_OF(1UL - 1ULL, ???));
    printtype2(&literals[4], &literals[5]);

    //static_assert(TYPE_OF(1UL - 1LL, long long));

    printf("#define TYPE_OF(E, T) _Generic(E, T : 1 , default: 0)\n");


    printf("\n");
    printf("//Comparision operators\n\n");
    for (int k = 0; k < ARRAYSIZE(comparision_operators); k++)
    {
        for (int i = 0; i < ARRAYSIZE(literals); i++)
        {
            for (int j = 0; j < ARRAYSIZE(literals); j++)
            {
                printf("static_assert(TYPE_OF(%s %s %s, ", literals[i].value, comparision_operators[k], literals[j].value);
                printf("int));\n");
                printf("static_assert(TYPE_OF(%s %s %s, int));\n", literals[i].value, comparision_operators[k], literals[j].value);
            }
        }
    }

    printf("\n");
    printf("//Relational operators\n\n");
    for (int k = 0; k < ARRAYSIZE(relational_operators); k++)
    {
        for (int i = 0; i < ARRAYSIZE(literals); i++)
        {
            for (int j = 0; j < ARRAYSIZE(literals); j++)
            {
                printf("static_assert(TYPE_OF(%s %s %s, ", literals[i].value, relational_operators[k], literals[j].value);
                printf("int));\n");
                printf("static_assert(TYPE_OF(%s %s %s, int));\n", literals[i].value, relational_operators[k], literals[j].value);
            }
        }
    }

    printf("\n");
    printf("// Aritmetic operators\n\n");
    for (int k = 0; k < ARRAYSIZE(arithemtic_operators); k++)
    {
        for (int i = 0; i < ARRAYSIZE(literals); i++)
        {
            for (int j = 0; j < ARRAYSIZE(literals); j++)
            {
                if (strcmp(arithemtic_operators[k], "%") == 0 &&
                    (is_float2(literals[i].type) || is_float2(literals[j].type))
                    )
                {
                    printf("//");
                }
                printf("static_assert(TYPE_OF(%s %s %s, ", literals[i].value, arithemtic_operators[k], literals[j].value);
                printf("%s", printtype2(&literals[i], &literals[j]));
                printf("));\n");
            }

        }
        printf("\n\n\n");
    }


}

#endif


#ifdef __linux__
#define TYPE_OF(E, T) _Generic(E, T : 1 , default: 0)

//Comparision operators

static_assert(TYPE_OF(1 == 1, int));
static_assert(TYPE_OF(1 == 1, int));
static_assert(TYPE_OF(1 == 1L, int));
static_assert(TYPE_OF(1 == 1L, int));
static_assert(TYPE_OF(1 == 1LL, int));
static_assert(TYPE_OF(1 == 1LL, int));
static_assert(TYPE_OF(1 == 1U, int));
static_assert(TYPE_OF(1 == 1U, int));
static_assert(TYPE_OF(1 == 1UL, int));
static_assert(TYPE_OF(1 == 1UL, int));
static_assert(TYPE_OF(1 == 1ULL, int));
static_assert(TYPE_OF(1 == 1ULL, int));
static_assert(TYPE_OF(1 == 1.0, int));
static_assert(TYPE_OF(1 == 1.0, int));
static_assert(TYPE_OF(1 == 1.0f, int));
static_assert(TYPE_OF(1 == 1.0f, int));
static_assert(TYPE_OF(1 == 1.0L, int));
static_assert(TYPE_OF(1 == 1.0L, int));
static_assert(TYPE_OF(1L == 1, int));
static_assert(TYPE_OF(1L == 1, int));
static_assert(TYPE_OF(1L == 1L, int));
static_assert(TYPE_OF(1L == 1L, int));
static_assert(TYPE_OF(1L == 1LL, int));
static_assert(TYPE_OF(1L == 1LL, int));
static_assert(TYPE_OF(1L == 1U, int));
static_assert(TYPE_OF(1L == 1U, int));
static_assert(TYPE_OF(1L == 1UL, int));
static_assert(TYPE_OF(1L == 1UL, int));
static_assert(TYPE_OF(1L == 1ULL, int));
static_assert(TYPE_OF(1L == 1ULL, int));
static_assert(TYPE_OF(1L == 1.0, int));
static_assert(TYPE_OF(1L == 1.0, int));
static_assert(TYPE_OF(1L == 1.0f, int));
static_assert(TYPE_OF(1L == 1.0f, int));
static_assert(TYPE_OF(1L == 1.0L, int));
static_assert(TYPE_OF(1L == 1.0L, int));
static_assert(TYPE_OF(1LL == 1, int));
static_assert(TYPE_OF(1LL == 1, int));
static_assert(TYPE_OF(1LL == 1L, int));
static_assert(TYPE_OF(1LL == 1L, int));
static_assert(TYPE_OF(1LL == 1LL, int));
static_assert(TYPE_OF(1LL == 1LL, int));
static_assert(TYPE_OF(1LL == 1U, int));
static_assert(TYPE_OF(1LL == 1U, int));
static_assert(TYPE_OF(1LL == 1UL, int));
static_assert(TYPE_OF(1LL == 1UL, int));
static_assert(TYPE_OF(1LL == 1ULL, int));
static_assert(TYPE_OF(1LL == 1ULL, int));
static_assert(TYPE_OF(1LL == 1.0, int));
static_assert(TYPE_OF(1LL == 1.0, int));
static_assert(TYPE_OF(1LL == 1.0f, int));
static_assert(TYPE_OF(1LL == 1.0f, int));
static_assert(TYPE_OF(1LL == 1.0L, int));
static_assert(TYPE_OF(1LL == 1.0L, int));
static_assert(TYPE_OF(1U == 1, int));
static_assert(TYPE_OF(1U == 1, int));
static_assert(TYPE_OF(1U == 1L, int));
static_assert(TYPE_OF(1U == 1L, int));
static_assert(TYPE_OF(1U == 1LL, int));
static_assert(TYPE_OF(1U == 1LL, int));
static_assert(TYPE_OF(1U == 1U, int));
static_assert(TYPE_OF(1U == 1U, int));
static_assert(TYPE_OF(1U == 1UL, int));
static_assert(TYPE_OF(1U == 1UL, int));
static_assert(TYPE_OF(1U == 1ULL, int));
static_assert(TYPE_OF(1U == 1ULL, int));
static_assert(TYPE_OF(1U == 1.0, int));
static_assert(TYPE_OF(1U == 1.0, int));
static_assert(TYPE_OF(1U == 1.0f, int));
static_assert(TYPE_OF(1U == 1.0f, int));
static_assert(TYPE_OF(1U == 1.0L, int));
static_assert(TYPE_OF(1U == 1.0L, int));
static_assert(TYPE_OF(1UL == 1, int));
static_assert(TYPE_OF(1UL == 1, int));
static_assert(TYPE_OF(1UL == 1L, int));
static_assert(TYPE_OF(1UL == 1L, int));
static_assert(TYPE_OF(1UL == 1LL, int));
static_assert(TYPE_OF(1UL == 1LL, int));
static_assert(TYPE_OF(1UL == 1U, int));
static_assert(TYPE_OF(1UL == 1U, int));
static_assert(TYPE_OF(1UL == 1UL, int));
static_assert(TYPE_OF(1UL == 1UL, int));
static_assert(TYPE_OF(1UL == 1ULL, int));
static_assert(TYPE_OF(1UL == 1ULL, int));
static_assert(TYPE_OF(1UL == 1.0, int));
static_assert(TYPE_OF(1UL == 1.0, int));
static_assert(TYPE_OF(1UL == 1.0f, int));
static_assert(TYPE_OF(1UL == 1.0f, int));
static_assert(TYPE_OF(1UL == 1.0L, int));
static_assert(TYPE_OF(1UL == 1.0L, int));
static_assert(TYPE_OF(1ULL == 1, int));
static_assert(TYPE_OF(1ULL == 1, int));
static_assert(TYPE_OF(1ULL == 1L, int));
static_assert(TYPE_OF(1ULL == 1L, int));
static_assert(TYPE_OF(1ULL == 1LL, int));
static_assert(TYPE_OF(1ULL == 1LL, int));
static_assert(TYPE_OF(1ULL == 1U, int));
static_assert(TYPE_OF(1ULL == 1U, int));
static_assert(TYPE_OF(1ULL == 1UL, int));
static_assert(TYPE_OF(1ULL == 1UL, int));
static_assert(TYPE_OF(1ULL == 1ULL, int));
static_assert(TYPE_OF(1ULL == 1ULL, int));
static_assert(TYPE_OF(1ULL == 1.0, int));
static_assert(TYPE_OF(1ULL == 1.0, int));
static_assert(TYPE_OF(1ULL == 1.0f, int));
static_assert(TYPE_OF(1ULL == 1.0f, int));
static_assert(TYPE_OF(1ULL == 1.0L, int));
static_assert(TYPE_OF(1ULL == 1.0L, int));
static_assert(TYPE_OF(1.0 == 1, int));
static_assert(TYPE_OF(1.0 == 1, int));
static_assert(TYPE_OF(1.0 == 1L, int));
static_assert(TYPE_OF(1.0 == 1L, int));
static_assert(TYPE_OF(1.0 == 1LL, int));
static_assert(TYPE_OF(1.0 == 1LL, int));
static_assert(TYPE_OF(1.0 == 1U, int));
static_assert(TYPE_OF(1.0 == 1U, int));
static_assert(TYPE_OF(1.0 == 1UL, int));
static_assert(TYPE_OF(1.0 == 1UL, int));
static_assert(TYPE_OF(1.0 == 1ULL, int));
static_assert(TYPE_OF(1.0 == 1ULL, int));
static_assert(TYPE_OF(1.0 == 1.0, int));
static_assert(TYPE_OF(1.0 == 1.0, int));
static_assert(TYPE_OF(1.0 == 1.0f, int));
static_assert(TYPE_OF(1.0 == 1.0f, int));
static_assert(TYPE_OF(1.0 == 1.0L, int));
static_assert(TYPE_OF(1.0 == 1.0L, int));
static_assert(TYPE_OF(1.0f == 1, int));
static_assert(TYPE_OF(1.0f == 1, int));
static_assert(TYPE_OF(1.0f == 1L, int));
static_assert(TYPE_OF(1.0f == 1L, int));
static_assert(TYPE_OF(1.0f == 1LL, int));
static_assert(TYPE_OF(1.0f == 1LL, int));
static_assert(TYPE_OF(1.0f == 1U, int));
static_assert(TYPE_OF(1.0f == 1U, int));
static_assert(TYPE_OF(1.0f == 1UL, int));
static_assert(TYPE_OF(1.0f == 1UL, int));
static_assert(TYPE_OF(1.0f == 1ULL, int));
static_assert(TYPE_OF(1.0f == 1ULL, int));
static_assert(TYPE_OF(1.0f == 1.0, int));
static_assert(TYPE_OF(1.0f == 1.0, int));
static_assert(TYPE_OF(1.0f == 1.0f, int));
static_assert(TYPE_OF(1.0f == 1.0f, int));
static_assert(TYPE_OF(1.0f == 1.0L, int));
static_assert(TYPE_OF(1.0f == 1.0L, int));
static_assert(TYPE_OF(1.0L == 1, int));
static_assert(TYPE_OF(1.0L == 1, int));
static_assert(TYPE_OF(1.0L == 1L, int));
static_assert(TYPE_OF(1.0L == 1L, int));
static_assert(TYPE_OF(1.0L == 1LL, int));
static_assert(TYPE_OF(1.0L == 1LL, int));
static_assert(TYPE_OF(1.0L == 1U, int));
static_assert(TYPE_OF(1.0L == 1U, int));
static_assert(TYPE_OF(1.0L == 1UL, int));
static_assert(TYPE_OF(1.0L == 1UL, int));
static_assert(TYPE_OF(1.0L == 1ULL, int));
static_assert(TYPE_OF(1.0L == 1ULL, int));
static_assert(TYPE_OF(1.0L == 1.0, int));
static_assert(TYPE_OF(1.0L == 1.0, int));
static_assert(TYPE_OF(1.0L == 1.0f, int));
static_assert(TYPE_OF(1.0L == 1.0f, int));
static_assert(TYPE_OF(1.0L == 1.0L, int));
static_assert(TYPE_OF(1.0L == 1.0L, int));
static_assert(TYPE_OF(1 != 1, int));
static_assert(TYPE_OF(1 != 1, int));
static_assert(TYPE_OF(1 != 1L, int));
static_assert(TYPE_OF(1 != 1L, int));
static_assert(TYPE_OF(1 != 1LL, int));
static_assert(TYPE_OF(1 != 1LL, int));
static_assert(TYPE_OF(1 != 1U, int));
static_assert(TYPE_OF(1 != 1U, int));
static_assert(TYPE_OF(1 != 1UL, int));
static_assert(TYPE_OF(1 != 1UL, int));
static_assert(TYPE_OF(1 != 1ULL, int));
static_assert(TYPE_OF(1 != 1ULL, int));
static_assert(TYPE_OF(1 != 1.0, int));
static_assert(TYPE_OF(1 != 1.0, int));
static_assert(TYPE_OF(1 != 1.0f, int));
static_assert(TYPE_OF(1 != 1.0f, int));
static_assert(TYPE_OF(1 != 1.0L, int));
static_assert(TYPE_OF(1 != 1.0L, int));
static_assert(TYPE_OF(1L != 1, int));
static_assert(TYPE_OF(1L != 1, int));
static_assert(TYPE_OF(1L != 1L, int));
static_assert(TYPE_OF(1L != 1L, int));
static_assert(TYPE_OF(1L != 1LL, int));
static_assert(TYPE_OF(1L != 1LL, int));
static_assert(TYPE_OF(1L != 1U, int));
static_assert(TYPE_OF(1L != 1U, int));
static_assert(TYPE_OF(1L != 1UL, int));
static_assert(TYPE_OF(1L != 1UL, int));
static_assert(TYPE_OF(1L != 1ULL, int));
static_assert(TYPE_OF(1L != 1ULL, int));
static_assert(TYPE_OF(1L != 1.0, int));
static_assert(TYPE_OF(1L != 1.0, int));
static_assert(TYPE_OF(1L != 1.0f, int));
static_assert(TYPE_OF(1L != 1.0f, int));
static_assert(TYPE_OF(1L != 1.0L, int));
static_assert(TYPE_OF(1L != 1.0L, int));
static_assert(TYPE_OF(1LL != 1, int));
static_assert(TYPE_OF(1LL != 1, int));
static_assert(TYPE_OF(1LL != 1L, int));
static_assert(TYPE_OF(1LL != 1L, int));
static_assert(TYPE_OF(1LL != 1LL, int));
static_assert(TYPE_OF(1LL != 1LL, int));
static_assert(TYPE_OF(1LL != 1U, int));
static_assert(TYPE_OF(1LL != 1U, int));
static_assert(TYPE_OF(1LL != 1UL, int));
static_assert(TYPE_OF(1LL != 1UL, int));
static_assert(TYPE_OF(1LL != 1ULL, int));
static_assert(TYPE_OF(1LL != 1ULL, int));
static_assert(TYPE_OF(1LL != 1.0, int));
static_assert(TYPE_OF(1LL != 1.0, int));
static_assert(TYPE_OF(1LL != 1.0f, int));
static_assert(TYPE_OF(1LL != 1.0f, int));
static_assert(TYPE_OF(1LL != 1.0L, int));
static_assert(TYPE_OF(1LL != 1.0L, int));
static_assert(TYPE_OF(1U != 1, int));
static_assert(TYPE_OF(1U != 1, int));
static_assert(TYPE_OF(1U != 1L, int));
static_assert(TYPE_OF(1U != 1L, int));
static_assert(TYPE_OF(1U != 1LL, int));
static_assert(TYPE_OF(1U != 1LL, int));
static_assert(TYPE_OF(1U != 1U, int));
static_assert(TYPE_OF(1U != 1U, int));
static_assert(TYPE_OF(1U != 1UL, int));
static_assert(TYPE_OF(1U != 1UL, int));
static_assert(TYPE_OF(1U != 1ULL, int));
static_assert(TYPE_OF(1U != 1ULL, int));
static_assert(TYPE_OF(1U != 1.0, int));
static_assert(TYPE_OF(1U != 1.0, int));
static_assert(TYPE_OF(1U != 1.0f, int));
static_assert(TYPE_OF(1U != 1.0f, int));
static_assert(TYPE_OF(1U != 1.0L, int));
static_assert(TYPE_OF(1U != 1.0L, int));
static_assert(TYPE_OF(1UL != 1, int));
static_assert(TYPE_OF(1UL != 1, int));
static_assert(TYPE_OF(1UL != 1L, int));
static_assert(TYPE_OF(1UL != 1L, int));
static_assert(TYPE_OF(1UL != 1LL, int));
static_assert(TYPE_OF(1UL != 1LL, int));
static_assert(TYPE_OF(1UL != 1U, int));
static_assert(TYPE_OF(1UL != 1U, int));
static_assert(TYPE_OF(1UL != 1UL, int));
static_assert(TYPE_OF(1UL != 1UL, int));
static_assert(TYPE_OF(1UL != 1ULL, int));
static_assert(TYPE_OF(1UL != 1ULL, int));
static_assert(TYPE_OF(1UL != 1.0, int));
static_assert(TYPE_OF(1UL != 1.0, int));
static_assert(TYPE_OF(1UL != 1.0f, int));
static_assert(TYPE_OF(1UL != 1.0f, int));
static_assert(TYPE_OF(1UL != 1.0L, int));
static_assert(TYPE_OF(1UL != 1.0L, int));
static_assert(TYPE_OF(1ULL != 1, int));
static_assert(TYPE_OF(1ULL != 1, int));
static_assert(TYPE_OF(1ULL != 1L, int));
static_assert(TYPE_OF(1ULL != 1L, int));
static_assert(TYPE_OF(1ULL != 1LL, int));
static_assert(TYPE_OF(1ULL != 1LL, int));
static_assert(TYPE_OF(1ULL != 1U, int));
static_assert(TYPE_OF(1ULL != 1U, int));
static_assert(TYPE_OF(1ULL != 1UL, int));
static_assert(TYPE_OF(1ULL != 1UL, int));
static_assert(TYPE_OF(1ULL != 1ULL, int));
static_assert(TYPE_OF(1ULL != 1ULL, int));
static_assert(TYPE_OF(1ULL != 1.0, int));
static_assert(TYPE_OF(1ULL != 1.0, int));
static_assert(TYPE_OF(1ULL != 1.0f, int));
static_assert(TYPE_OF(1ULL != 1.0f, int));
static_assert(TYPE_OF(1ULL != 1.0L, int));
static_assert(TYPE_OF(1ULL != 1.0L, int));
static_assert(TYPE_OF(1.0 != 1, int));
static_assert(TYPE_OF(1.0 != 1, int));
static_assert(TYPE_OF(1.0 != 1L, int));
static_assert(TYPE_OF(1.0 != 1L, int));
static_assert(TYPE_OF(1.0 != 1LL, int));
static_assert(TYPE_OF(1.0 != 1LL, int));
static_assert(TYPE_OF(1.0 != 1U, int));
static_assert(TYPE_OF(1.0 != 1U, int));
static_assert(TYPE_OF(1.0 != 1UL, int));
static_assert(TYPE_OF(1.0 != 1UL, int));
static_assert(TYPE_OF(1.0 != 1ULL, int));
static_assert(TYPE_OF(1.0 != 1ULL, int));
static_assert(TYPE_OF(1.0 != 1.0, int));
static_assert(TYPE_OF(1.0 != 1.0, int));
static_assert(TYPE_OF(1.0 != 1.0f, int));
static_assert(TYPE_OF(1.0 != 1.0f, int));
static_assert(TYPE_OF(1.0 != 1.0L, int));
static_assert(TYPE_OF(1.0 != 1.0L, int));
static_assert(TYPE_OF(1.0f != 1, int));
static_assert(TYPE_OF(1.0f != 1, int));
static_assert(TYPE_OF(1.0f != 1L, int));
static_assert(TYPE_OF(1.0f != 1L, int));
static_assert(TYPE_OF(1.0f != 1LL, int));
static_assert(TYPE_OF(1.0f != 1LL, int));
static_assert(TYPE_OF(1.0f != 1U, int));
static_assert(TYPE_OF(1.0f != 1U, int));
static_assert(TYPE_OF(1.0f != 1UL, int));
static_assert(TYPE_OF(1.0f != 1UL, int));
static_assert(TYPE_OF(1.0f != 1ULL, int));
static_assert(TYPE_OF(1.0f != 1ULL, int));
static_assert(TYPE_OF(1.0f != 1.0, int));
static_assert(TYPE_OF(1.0f != 1.0, int));
static_assert(TYPE_OF(1.0f != 1.0f, int));
static_assert(TYPE_OF(1.0f != 1.0f, int));
static_assert(TYPE_OF(1.0f != 1.0L, int));
static_assert(TYPE_OF(1.0f != 1.0L, int));
static_assert(TYPE_OF(1.0L != 1, int));
static_assert(TYPE_OF(1.0L != 1, int));
static_assert(TYPE_OF(1.0L != 1L, int));
static_assert(TYPE_OF(1.0L != 1L, int));
static_assert(TYPE_OF(1.0L != 1LL, int));
static_assert(TYPE_OF(1.0L != 1LL, int));
static_assert(TYPE_OF(1.0L != 1U, int));
static_assert(TYPE_OF(1.0L != 1U, int));
static_assert(TYPE_OF(1.0L != 1UL, int));
static_assert(TYPE_OF(1.0L != 1UL, int));
static_assert(TYPE_OF(1.0L != 1ULL, int));
static_assert(TYPE_OF(1.0L != 1ULL, int));
static_assert(TYPE_OF(1.0L != 1.0, int));
static_assert(TYPE_OF(1.0L != 1.0, int));
static_assert(TYPE_OF(1.0L != 1.0f, int));
static_assert(TYPE_OF(1.0L != 1.0f, int));
static_assert(TYPE_OF(1.0L != 1.0L, int));
static_assert(TYPE_OF(1.0L != 1.0L, int));

//Relational operators

static_assert(TYPE_OF(1 > 1, int));
static_assert(TYPE_OF(1 > 1, int));
static_assert(TYPE_OF(1 > 1L, int));
static_assert(TYPE_OF(1 > 1L, int));
static_assert(TYPE_OF(1 > 1LL, int));
static_assert(TYPE_OF(1 > 1LL, int));
static_assert(TYPE_OF(1 > 1U, int));
static_assert(TYPE_OF(1 > 1U, int));
static_assert(TYPE_OF(1 > 1UL, int));
static_assert(TYPE_OF(1 > 1UL, int));
static_assert(TYPE_OF(1 > 1ULL, int));
static_assert(TYPE_OF(1 > 1ULL, int));
static_assert(TYPE_OF(1 > 1.0, int));
static_assert(TYPE_OF(1 > 1.0, int));
static_assert(TYPE_OF(1 > 1.0f, int));
static_assert(TYPE_OF(1 > 1.0f, int));
static_assert(TYPE_OF(1 > 1.0L, int));
static_assert(TYPE_OF(1 > 1.0L, int));
static_assert(TYPE_OF(1L > 1, int));
static_assert(TYPE_OF(1L > 1, int));
static_assert(TYPE_OF(1L > 1L, int));
static_assert(TYPE_OF(1L > 1L, int));
static_assert(TYPE_OF(1L > 1LL, int));
static_assert(TYPE_OF(1L > 1LL, int));
static_assert(TYPE_OF(1L > 1U, int));
static_assert(TYPE_OF(1L > 1U, int));
static_assert(TYPE_OF(1L > 1UL, int));
static_assert(TYPE_OF(1L > 1UL, int));
static_assert(TYPE_OF(1L > 1ULL, int));
static_assert(TYPE_OF(1L > 1ULL, int));
static_assert(TYPE_OF(1L > 1.0, int));
static_assert(TYPE_OF(1L > 1.0, int));
static_assert(TYPE_OF(1L > 1.0f, int));
static_assert(TYPE_OF(1L > 1.0f, int));
static_assert(TYPE_OF(1L > 1.0L, int));
static_assert(TYPE_OF(1L > 1.0L, int));
static_assert(TYPE_OF(1LL > 1, int));
static_assert(TYPE_OF(1LL > 1, int));
static_assert(TYPE_OF(1LL > 1L, int));
static_assert(TYPE_OF(1LL > 1L, int));
static_assert(TYPE_OF(1LL > 1LL, int));
static_assert(TYPE_OF(1LL > 1LL, int));
static_assert(TYPE_OF(1LL > 1U, int));
static_assert(TYPE_OF(1LL > 1U, int));
static_assert(TYPE_OF(1LL > 1UL, int));
static_assert(TYPE_OF(1LL > 1UL, int));
static_assert(TYPE_OF(1LL > 1ULL, int));
static_assert(TYPE_OF(1LL > 1ULL, int));
static_assert(TYPE_OF(1LL > 1.0, int));
static_assert(TYPE_OF(1LL > 1.0, int));
static_assert(TYPE_OF(1LL > 1.0f, int));
static_assert(TYPE_OF(1LL > 1.0f, int));
static_assert(TYPE_OF(1LL > 1.0L, int));
static_assert(TYPE_OF(1LL > 1.0L, int));
static_assert(TYPE_OF(1U > 1, int));
static_assert(TYPE_OF(1U > 1, int));
static_assert(TYPE_OF(1U > 1L, int));
static_assert(TYPE_OF(1U > 1L, int));
static_assert(TYPE_OF(1U > 1LL, int));
static_assert(TYPE_OF(1U > 1LL, int));
static_assert(TYPE_OF(1U > 1U, int));
static_assert(TYPE_OF(1U > 1U, int));
static_assert(TYPE_OF(1U > 1UL, int));
static_assert(TYPE_OF(1U > 1UL, int));
static_assert(TYPE_OF(1U > 1ULL, int));
static_assert(TYPE_OF(1U > 1ULL, int));
static_assert(TYPE_OF(1U > 1.0, int));
static_assert(TYPE_OF(1U > 1.0, int));
static_assert(TYPE_OF(1U > 1.0f, int));
static_assert(TYPE_OF(1U > 1.0f, int));
static_assert(TYPE_OF(1U > 1.0L, int));
static_assert(TYPE_OF(1U > 1.0L, int));
static_assert(TYPE_OF(1UL > 1, int));
static_assert(TYPE_OF(1UL > 1, int));
static_assert(TYPE_OF(1UL > 1L, int));
static_assert(TYPE_OF(1UL > 1L, int));
static_assert(TYPE_OF(1UL > 1LL, int));
static_assert(TYPE_OF(1UL > 1LL, int));
static_assert(TYPE_OF(1UL > 1U, int));
static_assert(TYPE_OF(1UL > 1U, int));
static_assert(TYPE_OF(1UL > 1UL, int));
static_assert(TYPE_OF(1UL > 1UL, int));
static_assert(TYPE_OF(1UL > 1ULL, int));
static_assert(TYPE_OF(1UL > 1ULL, int));
static_assert(TYPE_OF(1UL > 1.0, int));
static_assert(TYPE_OF(1UL > 1.0, int));
static_assert(TYPE_OF(1UL > 1.0f, int));
static_assert(TYPE_OF(1UL > 1.0f, int));
static_assert(TYPE_OF(1UL > 1.0L, int));
static_assert(TYPE_OF(1UL > 1.0L, int));
static_assert(TYPE_OF(1ULL > 1, int));
static_assert(TYPE_OF(1ULL > 1, int));
static_assert(TYPE_OF(1ULL > 1L, int));
static_assert(TYPE_OF(1ULL > 1L, int));
static_assert(TYPE_OF(1ULL > 1LL, int));
static_assert(TYPE_OF(1ULL > 1LL, int));
static_assert(TYPE_OF(1ULL > 1U, int));
static_assert(TYPE_OF(1ULL > 1U, int));
static_assert(TYPE_OF(1ULL > 1UL, int));
static_assert(TYPE_OF(1ULL > 1UL, int));
static_assert(TYPE_OF(1ULL > 1ULL, int));
static_assert(TYPE_OF(1ULL > 1ULL, int));
static_assert(TYPE_OF(1ULL > 1.0, int));
static_assert(TYPE_OF(1ULL > 1.0, int));
static_assert(TYPE_OF(1ULL > 1.0f, int));
static_assert(TYPE_OF(1ULL > 1.0f, int));
static_assert(TYPE_OF(1ULL > 1.0L, int));
static_assert(TYPE_OF(1ULL > 1.0L, int));
static_assert(TYPE_OF(1.0 > 1, int));
static_assert(TYPE_OF(1.0 > 1, int));
static_assert(TYPE_OF(1.0 > 1L, int));
static_assert(TYPE_OF(1.0 > 1L, int));
static_assert(TYPE_OF(1.0 > 1LL, int));
static_assert(TYPE_OF(1.0 > 1LL, int));
static_assert(TYPE_OF(1.0 > 1U, int));
static_assert(TYPE_OF(1.0 > 1U, int));
static_assert(TYPE_OF(1.0 > 1UL, int));
static_assert(TYPE_OF(1.0 > 1UL, int));
static_assert(TYPE_OF(1.0 > 1ULL, int));
static_assert(TYPE_OF(1.0 > 1ULL, int));
static_assert(TYPE_OF(1.0 > 1.0, int));
static_assert(TYPE_OF(1.0 > 1.0, int));
static_assert(TYPE_OF(1.0 > 1.0f, int));
static_assert(TYPE_OF(1.0 > 1.0f, int));
static_assert(TYPE_OF(1.0 > 1.0L, int));
static_assert(TYPE_OF(1.0 > 1.0L, int));
static_assert(TYPE_OF(1.0f > 1, int));
static_assert(TYPE_OF(1.0f > 1, int));
static_assert(TYPE_OF(1.0f > 1L, int));
static_assert(TYPE_OF(1.0f > 1L, int));
static_assert(TYPE_OF(1.0f > 1LL, int));
static_assert(TYPE_OF(1.0f > 1LL, int));
static_assert(TYPE_OF(1.0f > 1U, int));
static_assert(TYPE_OF(1.0f > 1U, int));
static_assert(TYPE_OF(1.0f > 1UL, int));
static_assert(TYPE_OF(1.0f > 1UL, int));
static_assert(TYPE_OF(1.0f > 1ULL, int));
static_assert(TYPE_OF(1.0f > 1ULL, int));
static_assert(TYPE_OF(1.0f > 1.0, int));
static_assert(TYPE_OF(1.0f > 1.0, int));
static_assert(TYPE_OF(1.0f > 1.0f, int));
static_assert(TYPE_OF(1.0f > 1.0f, int));
static_assert(TYPE_OF(1.0f > 1.0L, int));
static_assert(TYPE_OF(1.0f > 1.0L, int));
static_assert(TYPE_OF(1.0L > 1, int));
static_assert(TYPE_OF(1.0L > 1, int));
static_assert(TYPE_OF(1.0L > 1L, int));
static_assert(TYPE_OF(1.0L > 1L, int));
static_assert(TYPE_OF(1.0L > 1LL, int));
static_assert(TYPE_OF(1.0L > 1LL, int));
static_assert(TYPE_OF(1.0L > 1U, int));
static_assert(TYPE_OF(1.0L > 1U, int));
static_assert(TYPE_OF(1.0L > 1UL, int));
static_assert(TYPE_OF(1.0L > 1UL, int));
static_assert(TYPE_OF(1.0L > 1ULL, int));
static_assert(TYPE_OF(1.0L > 1ULL, int));
static_assert(TYPE_OF(1.0L > 1.0, int));
static_assert(TYPE_OF(1.0L > 1.0, int));
static_assert(TYPE_OF(1.0L > 1.0f, int));
static_assert(TYPE_OF(1.0L > 1.0f, int));
static_assert(TYPE_OF(1.0L > 1.0L, int));
static_assert(TYPE_OF(1.0L > 1.0L, int));
static_assert(TYPE_OF(1 >= 1, int));
static_assert(TYPE_OF(1 >= 1, int));
static_assert(TYPE_OF(1 >= 1L, int));
static_assert(TYPE_OF(1 >= 1L, int));
static_assert(TYPE_OF(1 >= 1LL, int));
static_assert(TYPE_OF(1 >= 1LL, int));
static_assert(TYPE_OF(1 >= 1U, int));
static_assert(TYPE_OF(1 >= 1U, int));
static_assert(TYPE_OF(1 >= 1UL, int));
static_assert(TYPE_OF(1 >= 1UL, int));
static_assert(TYPE_OF(1 >= 1ULL, int));
static_assert(TYPE_OF(1 >= 1ULL, int));
static_assert(TYPE_OF(1 >= 1.0, int));
static_assert(TYPE_OF(1 >= 1.0, int));
static_assert(TYPE_OF(1 >= 1.0f, int));
static_assert(TYPE_OF(1 >= 1.0f, int));
static_assert(TYPE_OF(1 >= 1.0L, int));
static_assert(TYPE_OF(1 >= 1.0L, int));
static_assert(TYPE_OF(1L >= 1, int));
static_assert(TYPE_OF(1L >= 1, int));
static_assert(TYPE_OF(1L >= 1L, int));
static_assert(TYPE_OF(1L >= 1L, int));
static_assert(TYPE_OF(1L >= 1LL, int));
static_assert(TYPE_OF(1L >= 1LL, int));
static_assert(TYPE_OF(1L >= 1U, int));
static_assert(TYPE_OF(1L >= 1U, int));
static_assert(TYPE_OF(1L >= 1UL, int));
static_assert(TYPE_OF(1L >= 1UL, int));
static_assert(TYPE_OF(1L >= 1ULL, int));
static_assert(TYPE_OF(1L >= 1ULL, int));
static_assert(TYPE_OF(1L >= 1.0, int));
static_assert(TYPE_OF(1L >= 1.0, int));
static_assert(TYPE_OF(1L >= 1.0f, int));
static_assert(TYPE_OF(1L >= 1.0f, int));
static_assert(TYPE_OF(1L >= 1.0L, int));
static_assert(TYPE_OF(1L >= 1.0L, int));
static_assert(TYPE_OF(1LL >= 1, int));
static_assert(TYPE_OF(1LL >= 1, int));
static_assert(TYPE_OF(1LL >= 1L, int));
static_assert(TYPE_OF(1LL >= 1L, int));
static_assert(TYPE_OF(1LL >= 1LL, int));
static_assert(TYPE_OF(1LL >= 1LL, int));
static_assert(TYPE_OF(1LL >= 1U, int));
static_assert(TYPE_OF(1LL >= 1U, int));
static_assert(TYPE_OF(1LL >= 1UL, int));
static_assert(TYPE_OF(1LL >= 1UL, int));
static_assert(TYPE_OF(1LL >= 1ULL, int));
static_assert(TYPE_OF(1LL >= 1ULL, int));
static_assert(TYPE_OF(1LL >= 1.0, int));
static_assert(TYPE_OF(1LL >= 1.0, int));
static_assert(TYPE_OF(1LL >= 1.0f, int));
static_assert(TYPE_OF(1LL >= 1.0f, int));
static_assert(TYPE_OF(1LL >= 1.0L, int));
static_assert(TYPE_OF(1LL >= 1.0L, int));
static_assert(TYPE_OF(1U >= 1, int));
static_assert(TYPE_OF(1U >= 1, int));
static_assert(TYPE_OF(1U >= 1L, int));
static_assert(TYPE_OF(1U >= 1L, int));
static_assert(TYPE_OF(1U >= 1LL, int));
static_assert(TYPE_OF(1U >= 1LL, int));
static_assert(TYPE_OF(1U >= 1U, int));
static_assert(TYPE_OF(1U >= 1U, int));
static_assert(TYPE_OF(1U >= 1UL, int));
static_assert(TYPE_OF(1U >= 1UL, int));
static_assert(TYPE_OF(1U >= 1ULL, int));
static_assert(TYPE_OF(1U >= 1ULL, int));
static_assert(TYPE_OF(1U >= 1.0, int));
static_assert(TYPE_OF(1U >= 1.0, int));
static_assert(TYPE_OF(1U >= 1.0f, int));
static_assert(TYPE_OF(1U >= 1.0f, int));
static_assert(TYPE_OF(1U >= 1.0L, int));
static_assert(TYPE_OF(1U >= 1.0L, int));
static_assert(TYPE_OF(1UL >= 1, int));
static_assert(TYPE_OF(1UL >= 1, int));
static_assert(TYPE_OF(1UL >= 1L, int));
static_assert(TYPE_OF(1UL >= 1L, int));
static_assert(TYPE_OF(1UL >= 1LL, int));
static_assert(TYPE_OF(1UL >= 1LL, int));
static_assert(TYPE_OF(1UL >= 1U, int));
static_assert(TYPE_OF(1UL >= 1U, int));
static_assert(TYPE_OF(1UL >= 1UL, int));
static_assert(TYPE_OF(1UL >= 1UL, int));
static_assert(TYPE_OF(1UL >= 1ULL, int));
static_assert(TYPE_OF(1UL >= 1ULL, int));
static_assert(TYPE_OF(1UL >= 1.0, int));
static_assert(TYPE_OF(1UL >= 1.0, int));
static_assert(TYPE_OF(1UL >= 1.0f, int));
static_assert(TYPE_OF(1UL >= 1.0f, int));
static_assert(TYPE_OF(1UL >= 1.0L, int));
static_assert(TYPE_OF(1UL >= 1.0L, int));
static_assert(TYPE_OF(1ULL >= 1, int));
static_assert(TYPE_OF(1ULL >= 1, int));
static_assert(TYPE_OF(1ULL >= 1L, int));
static_assert(TYPE_OF(1ULL >= 1L, int));
static_assert(TYPE_OF(1ULL >= 1LL, int));
static_assert(TYPE_OF(1ULL >= 1LL, int));
static_assert(TYPE_OF(1ULL >= 1U, int));
static_assert(TYPE_OF(1ULL >= 1U, int));
static_assert(TYPE_OF(1ULL >= 1UL, int));
static_assert(TYPE_OF(1ULL >= 1UL, int));
static_assert(TYPE_OF(1ULL >= 1ULL, int));
static_assert(TYPE_OF(1ULL >= 1ULL, int));
static_assert(TYPE_OF(1ULL >= 1.0, int));
static_assert(TYPE_OF(1ULL >= 1.0, int));
static_assert(TYPE_OF(1ULL >= 1.0f, int));
static_assert(TYPE_OF(1ULL >= 1.0f, int));
static_assert(TYPE_OF(1ULL >= 1.0L, int));
static_assert(TYPE_OF(1ULL >= 1.0L, int));
static_assert(TYPE_OF(1.0 >= 1, int));
static_assert(TYPE_OF(1.0 >= 1, int));
static_assert(TYPE_OF(1.0 >= 1L, int));
static_assert(TYPE_OF(1.0 >= 1L, int));
static_assert(TYPE_OF(1.0 >= 1LL, int));
static_assert(TYPE_OF(1.0 >= 1LL, int));
static_assert(TYPE_OF(1.0 >= 1U, int));
static_assert(TYPE_OF(1.0 >= 1U, int));
static_assert(TYPE_OF(1.0 >= 1UL, int));
static_assert(TYPE_OF(1.0 >= 1UL, int));
static_assert(TYPE_OF(1.0 >= 1ULL, int));
static_assert(TYPE_OF(1.0 >= 1ULL, int));
static_assert(TYPE_OF(1.0 >= 1.0, int));
static_assert(TYPE_OF(1.0 >= 1.0, int));
static_assert(TYPE_OF(1.0 >= 1.0f, int));
static_assert(TYPE_OF(1.0 >= 1.0f, int));
static_assert(TYPE_OF(1.0 >= 1.0L, int));
static_assert(TYPE_OF(1.0 >= 1.0L, int));
static_assert(TYPE_OF(1.0f >= 1, int));
static_assert(TYPE_OF(1.0f >= 1, int));
static_assert(TYPE_OF(1.0f >= 1L, int));
static_assert(TYPE_OF(1.0f >= 1L, int));
static_assert(TYPE_OF(1.0f >= 1LL, int));
static_assert(TYPE_OF(1.0f >= 1LL, int));
static_assert(TYPE_OF(1.0f >= 1U, int));
static_assert(TYPE_OF(1.0f >= 1U, int));
static_assert(TYPE_OF(1.0f >= 1UL, int));
static_assert(TYPE_OF(1.0f >= 1UL, int));
static_assert(TYPE_OF(1.0f >= 1ULL, int));
static_assert(TYPE_OF(1.0f >= 1ULL, int));
static_assert(TYPE_OF(1.0f >= 1.0, int));
static_assert(TYPE_OF(1.0f >= 1.0, int));
static_assert(TYPE_OF(1.0f >= 1.0f, int));
static_assert(TYPE_OF(1.0f >= 1.0f, int));
static_assert(TYPE_OF(1.0f >= 1.0L, int));
static_assert(TYPE_OF(1.0f >= 1.0L, int));
static_assert(TYPE_OF(1.0L >= 1, int));
static_assert(TYPE_OF(1.0L >= 1, int));
static_assert(TYPE_OF(1.0L >= 1L, int));
static_assert(TYPE_OF(1.0L >= 1L, int));
static_assert(TYPE_OF(1.0L >= 1LL, int));
static_assert(TYPE_OF(1.0L >= 1LL, int));
static_assert(TYPE_OF(1.0L >= 1U, int));
static_assert(TYPE_OF(1.0L >= 1U, int));
static_assert(TYPE_OF(1.0L >= 1UL, int));
static_assert(TYPE_OF(1.0L >= 1UL, int));
static_assert(TYPE_OF(1.0L >= 1ULL, int));
static_assert(TYPE_OF(1.0L >= 1ULL, int));
static_assert(TYPE_OF(1.0L >= 1.0, int));
static_assert(TYPE_OF(1.0L >= 1.0, int));
static_assert(TYPE_OF(1.0L >= 1.0f, int));
static_assert(TYPE_OF(1.0L >= 1.0f, int));
static_assert(TYPE_OF(1.0L >= 1.0L, int));
static_assert(TYPE_OF(1.0L >= 1.0L, int));
static_assert(TYPE_OF(1 < 1, int));
static_assert(TYPE_OF(1 < 1, int));
static_assert(TYPE_OF(1 < 1L, int));
static_assert(TYPE_OF(1 < 1L, int));
static_assert(TYPE_OF(1 < 1LL, int));
static_assert(TYPE_OF(1 < 1LL, int));
static_assert(TYPE_OF(1 < 1U, int));
static_assert(TYPE_OF(1 < 1U, int));
static_assert(TYPE_OF(1 < 1UL, int));
static_assert(TYPE_OF(1 < 1UL, int));
static_assert(TYPE_OF(1 < 1ULL, int));
static_assert(TYPE_OF(1 < 1ULL, int));
static_assert(TYPE_OF(1 < 1.0, int));
static_assert(TYPE_OF(1 < 1.0, int));
static_assert(TYPE_OF(1 < 1.0f, int));
static_assert(TYPE_OF(1 < 1.0f, int));
static_assert(TYPE_OF(1 < 1.0L, int));
static_assert(TYPE_OF(1 < 1.0L, int));
static_assert(TYPE_OF(1L < 1, int));
static_assert(TYPE_OF(1L < 1, int));
static_assert(TYPE_OF(1L < 1L, int));
static_assert(TYPE_OF(1L < 1L, int));
static_assert(TYPE_OF(1L < 1LL, int));
static_assert(TYPE_OF(1L < 1LL, int));
static_assert(TYPE_OF(1L < 1U, int));
static_assert(TYPE_OF(1L < 1U, int));
static_assert(TYPE_OF(1L < 1UL, int));
static_assert(TYPE_OF(1L < 1UL, int));
static_assert(TYPE_OF(1L < 1ULL, int));
static_assert(TYPE_OF(1L < 1ULL, int));
static_assert(TYPE_OF(1L < 1.0, int));
static_assert(TYPE_OF(1L < 1.0, int));
static_assert(TYPE_OF(1L < 1.0f, int));
static_assert(TYPE_OF(1L < 1.0f, int));
static_assert(TYPE_OF(1L < 1.0L, int));
static_assert(TYPE_OF(1L < 1.0L, int));
static_assert(TYPE_OF(1LL < 1, int));
static_assert(TYPE_OF(1LL < 1, int));
static_assert(TYPE_OF(1LL < 1L, int));
static_assert(TYPE_OF(1LL < 1L, int));
static_assert(TYPE_OF(1LL < 1LL, int));
static_assert(TYPE_OF(1LL < 1LL, int));
static_assert(TYPE_OF(1LL < 1U, int));
static_assert(TYPE_OF(1LL < 1U, int));
static_assert(TYPE_OF(1LL < 1UL, int));
static_assert(TYPE_OF(1LL < 1UL, int));
static_assert(TYPE_OF(1LL < 1ULL, int));
static_assert(TYPE_OF(1LL < 1ULL, int));
static_assert(TYPE_OF(1LL < 1.0, int));
static_assert(TYPE_OF(1LL < 1.0, int));
static_assert(TYPE_OF(1LL < 1.0f, int));
static_assert(TYPE_OF(1LL < 1.0f, int));
static_assert(TYPE_OF(1LL < 1.0L, int));
static_assert(TYPE_OF(1LL < 1.0L, int));
static_assert(TYPE_OF(1U < 1, int));
static_assert(TYPE_OF(1U < 1, int));
static_assert(TYPE_OF(1U < 1L, int));
static_assert(TYPE_OF(1U < 1L, int));
static_assert(TYPE_OF(1U < 1LL, int));
static_assert(TYPE_OF(1U < 1LL, int));
static_assert(TYPE_OF(1U < 1U, int));
static_assert(TYPE_OF(1U < 1U, int));
static_assert(TYPE_OF(1U < 1UL, int));
static_assert(TYPE_OF(1U < 1UL, int));
static_assert(TYPE_OF(1U < 1ULL, int));
static_assert(TYPE_OF(1U < 1ULL, int));
static_assert(TYPE_OF(1U < 1.0, int));
static_assert(TYPE_OF(1U < 1.0, int));
static_assert(TYPE_OF(1U < 1.0f, int));
static_assert(TYPE_OF(1U < 1.0f, int));
static_assert(TYPE_OF(1U < 1.0L, int));
static_assert(TYPE_OF(1U < 1.0L, int));
static_assert(TYPE_OF(1UL < 1, int));
static_assert(TYPE_OF(1UL < 1, int));
static_assert(TYPE_OF(1UL < 1L, int));
static_assert(TYPE_OF(1UL < 1L, int));
static_assert(TYPE_OF(1UL < 1LL, int));
static_assert(TYPE_OF(1UL < 1LL, int));
static_assert(TYPE_OF(1UL < 1U, int));
static_assert(TYPE_OF(1UL < 1U, int));
static_assert(TYPE_OF(1UL < 1UL, int));
static_assert(TYPE_OF(1UL < 1UL, int));
static_assert(TYPE_OF(1UL < 1ULL, int));
static_assert(TYPE_OF(1UL < 1ULL, int));
static_assert(TYPE_OF(1UL < 1.0, int));
static_assert(TYPE_OF(1UL < 1.0, int));
static_assert(TYPE_OF(1UL < 1.0f, int));
static_assert(TYPE_OF(1UL < 1.0f, int));
static_assert(TYPE_OF(1UL < 1.0L, int));
static_assert(TYPE_OF(1UL < 1.0L, int));
static_assert(TYPE_OF(1ULL < 1, int));
static_assert(TYPE_OF(1ULL < 1, int));
static_assert(TYPE_OF(1ULL < 1L, int));
static_assert(TYPE_OF(1ULL < 1L, int));
static_assert(TYPE_OF(1ULL < 1LL, int));
static_assert(TYPE_OF(1ULL < 1LL, int));
static_assert(TYPE_OF(1ULL < 1U, int));
static_assert(TYPE_OF(1ULL < 1U, int));
static_assert(TYPE_OF(1ULL < 1UL, int));
static_assert(TYPE_OF(1ULL < 1UL, int));
static_assert(TYPE_OF(1ULL < 1ULL, int));
static_assert(TYPE_OF(1ULL < 1ULL, int));
static_assert(TYPE_OF(1ULL < 1.0, int));
static_assert(TYPE_OF(1ULL < 1.0, int));
static_assert(TYPE_OF(1ULL < 1.0f, int));
static_assert(TYPE_OF(1ULL < 1.0f, int));
static_assert(TYPE_OF(1ULL < 1.0L, int));
static_assert(TYPE_OF(1ULL < 1.0L, int));
static_assert(TYPE_OF(1.0 < 1, int));
static_assert(TYPE_OF(1.0 < 1, int));
static_assert(TYPE_OF(1.0 < 1L, int));
static_assert(TYPE_OF(1.0 < 1L, int));
static_assert(TYPE_OF(1.0 < 1LL, int));
static_assert(TYPE_OF(1.0 < 1LL, int));
static_assert(TYPE_OF(1.0 < 1U, int));
static_assert(TYPE_OF(1.0 < 1U, int));
static_assert(TYPE_OF(1.0 < 1UL, int));
static_assert(TYPE_OF(1.0 < 1UL, int));
static_assert(TYPE_OF(1.0 < 1ULL, int));
static_assert(TYPE_OF(1.0 < 1ULL, int));
static_assert(TYPE_OF(1.0 < 1.0, int));
static_assert(TYPE_OF(1.0 < 1.0, int));
static_assert(TYPE_OF(1.0 < 1.0f, int));
static_assert(TYPE_OF(1.0 < 1.0f, int));
static_assert(TYPE_OF(1.0 < 1.0L, int));
static_assert(TYPE_OF(1.0 < 1.0L, int));
static_assert(TYPE_OF(1.0f < 1, int));
static_assert(TYPE_OF(1.0f < 1, int));
static_assert(TYPE_OF(1.0f < 1L, int));
static_assert(TYPE_OF(1.0f < 1L, int));
static_assert(TYPE_OF(1.0f < 1LL, int));
static_assert(TYPE_OF(1.0f < 1LL, int));
static_assert(TYPE_OF(1.0f < 1U, int));
static_assert(TYPE_OF(1.0f < 1U, int));
static_assert(TYPE_OF(1.0f < 1UL, int));
static_assert(TYPE_OF(1.0f < 1UL, int));
static_assert(TYPE_OF(1.0f < 1ULL, int));
static_assert(TYPE_OF(1.0f < 1ULL, int));
static_assert(TYPE_OF(1.0f < 1.0, int));
static_assert(TYPE_OF(1.0f < 1.0, int));
static_assert(TYPE_OF(1.0f < 1.0f, int));
static_assert(TYPE_OF(1.0f < 1.0f, int));
static_assert(TYPE_OF(1.0f < 1.0L, int));
static_assert(TYPE_OF(1.0f < 1.0L, int));
static_assert(TYPE_OF(1.0L < 1, int));
static_assert(TYPE_OF(1.0L < 1, int));
static_assert(TYPE_OF(1.0L < 1L, int));
static_assert(TYPE_OF(1.0L < 1L, int));
static_assert(TYPE_OF(1.0L < 1LL, int));
static_assert(TYPE_OF(1.0L < 1LL, int));
static_assert(TYPE_OF(1.0L < 1U, int));
static_assert(TYPE_OF(1.0L < 1U, int));
static_assert(TYPE_OF(1.0L < 1UL, int));
static_assert(TYPE_OF(1.0L < 1UL, int));
static_assert(TYPE_OF(1.0L < 1ULL, int));
static_assert(TYPE_OF(1.0L < 1ULL, int));
static_assert(TYPE_OF(1.0L < 1.0, int));
static_assert(TYPE_OF(1.0L < 1.0, int));
static_assert(TYPE_OF(1.0L < 1.0f, int));
static_assert(TYPE_OF(1.0L < 1.0f, int));
static_assert(TYPE_OF(1.0L < 1.0L, int));
static_assert(TYPE_OF(1.0L < 1.0L, int));
static_assert(TYPE_OF(1 <= 1, int));
static_assert(TYPE_OF(1 <= 1, int));
static_assert(TYPE_OF(1 <= 1L, int));
static_assert(TYPE_OF(1 <= 1L, int));
static_assert(TYPE_OF(1 <= 1LL, int));
static_assert(TYPE_OF(1 <= 1LL, int));
static_assert(TYPE_OF(1 <= 1U, int));
static_assert(TYPE_OF(1 <= 1U, int));
static_assert(TYPE_OF(1 <= 1UL, int));
static_assert(TYPE_OF(1 <= 1UL, int));
static_assert(TYPE_OF(1 <= 1ULL, int));
static_assert(TYPE_OF(1 <= 1ULL, int));
static_assert(TYPE_OF(1 <= 1.0, int));
static_assert(TYPE_OF(1 <= 1.0, int));
static_assert(TYPE_OF(1 <= 1.0f, int));
static_assert(TYPE_OF(1 <= 1.0f, int));
static_assert(TYPE_OF(1 <= 1.0L, int));
static_assert(TYPE_OF(1 <= 1.0L, int));
static_assert(TYPE_OF(1L <= 1, int));
static_assert(TYPE_OF(1L <= 1, int));
static_assert(TYPE_OF(1L <= 1L, int));
static_assert(TYPE_OF(1L <= 1L, int));
static_assert(TYPE_OF(1L <= 1LL, int));
static_assert(TYPE_OF(1L <= 1LL, int));
static_assert(TYPE_OF(1L <= 1U, int));
static_assert(TYPE_OF(1L <= 1U, int));
static_assert(TYPE_OF(1L <= 1UL, int));
static_assert(TYPE_OF(1L <= 1UL, int));
static_assert(TYPE_OF(1L <= 1ULL, int));
static_assert(TYPE_OF(1L <= 1ULL, int));
static_assert(TYPE_OF(1L <= 1.0, int));
static_assert(TYPE_OF(1L <= 1.0, int));
static_assert(TYPE_OF(1L <= 1.0f, int));
static_assert(TYPE_OF(1L <= 1.0f, int));
static_assert(TYPE_OF(1L <= 1.0L, int));
static_assert(TYPE_OF(1L <= 1.0L, int));
static_assert(TYPE_OF(1LL <= 1, int));
static_assert(TYPE_OF(1LL <= 1, int));
static_assert(TYPE_OF(1LL <= 1L, int));
static_assert(TYPE_OF(1LL <= 1L, int));
static_assert(TYPE_OF(1LL <= 1LL, int));
static_assert(TYPE_OF(1LL <= 1LL, int));
static_assert(TYPE_OF(1LL <= 1U, int));
static_assert(TYPE_OF(1LL <= 1U, int));
static_assert(TYPE_OF(1LL <= 1UL, int));
static_assert(TYPE_OF(1LL <= 1UL, int));
static_assert(TYPE_OF(1LL <= 1ULL, int));
static_assert(TYPE_OF(1LL <= 1ULL, int));
static_assert(TYPE_OF(1LL <= 1.0, int));
static_assert(TYPE_OF(1LL <= 1.0, int));
static_assert(TYPE_OF(1LL <= 1.0f, int));
static_assert(TYPE_OF(1LL <= 1.0f, int));
static_assert(TYPE_OF(1LL <= 1.0L, int));
static_assert(TYPE_OF(1LL <= 1.0L, int));
static_assert(TYPE_OF(1U <= 1, int));
static_assert(TYPE_OF(1U <= 1, int));
static_assert(TYPE_OF(1U <= 1L, int));
static_assert(TYPE_OF(1U <= 1L, int));
static_assert(TYPE_OF(1U <= 1LL, int));
static_assert(TYPE_OF(1U <= 1LL, int));
static_assert(TYPE_OF(1U <= 1U, int));
static_assert(TYPE_OF(1U <= 1U, int));
static_assert(TYPE_OF(1U <= 1UL, int));
static_assert(TYPE_OF(1U <= 1UL, int));
static_assert(TYPE_OF(1U <= 1ULL, int));
static_assert(TYPE_OF(1U <= 1ULL, int));
static_assert(TYPE_OF(1U <= 1.0, int));
static_assert(TYPE_OF(1U <= 1.0, int));
static_assert(TYPE_OF(1U <= 1.0f, int));
static_assert(TYPE_OF(1U <= 1.0f, int));
static_assert(TYPE_OF(1U <= 1.0L, int));
static_assert(TYPE_OF(1U <= 1.0L, int));
static_assert(TYPE_OF(1UL <= 1, int));
static_assert(TYPE_OF(1UL <= 1, int));
static_assert(TYPE_OF(1UL <= 1L, int));
static_assert(TYPE_OF(1UL <= 1L, int));
static_assert(TYPE_OF(1UL <= 1LL, int));
static_assert(TYPE_OF(1UL <= 1LL, int));
static_assert(TYPE_OF(1UL <= 1U, int));
static_assert(TYPE_OF(1UL <= 1U, int));
static_assert(TYPE_OF(1UL <= 1UL, int));
static_assert(TYPE_OF(1UL <= 1UL, int));
static_assert(TYPE_OF(1UL <= 1ULL, int));
static_assert(TYPE_OF(1UL <= 1ULL, int));
static_assert(TYPE_OF(1UL <= 1.0, int));
static_assert(TYPE_OF(1UL <= 1.0, int));
static_assert(TYPE_OF(1UL <= 1.0f, int));
static_assert(TYPE_OF(1UL <= 1.0f, int));
static_assert(TYPE_OF(1UL <= 1.0L, int));
static_assert(TYPE_OF(1UL <= 1.0L, int));
static_assert(TYPE_OF(1ULL <= 1, int));
static_assert(TYPE_OF(1ULL <= 1, int));
static_assert(TYPE_OF(1ULL <= 1L, int));
static_assert(TYPE_OF(1ULL <= 1L, int));
static_assert(TYPE_OF(1ULL <= 1LL, int));
static_assert(TYPE_OF(1ULL <= 1LL, int));
static_assert(TYPE_OF(1ULL <= 1U, int));
static_assert(TYPE_OF(1ULL <= 1U, int));
static_assert(TYPE_OF(1ULL <= 1UL, int));
static_assert(TYPE_OF(1ULL <= 1UL, int));
static_assert(TYPE_OF(1ULL <= 1ULL, int));
static_assert(TYPE_OF(1ULL <= 1ULL, int));
static_assert(TYPE_OF(1ULL <= 1.0, int));
static_assert(TYPE_OF(1ULL <= 1.0, int));
static_assert(TYPE_OF(1ULL <= 1.0f, int));
static_assert(TYPE_OF(1ULL <= 1.0f, int));
static_assert(TYPE_OF(1ULL <= 1.0L, int));
static_assert(TYPE_OF(1ULL <= 1.0L, int));
static_assert(TYPE_OF(1.0 <= 1, int));
static_assert(TYPE_OF(1.0 <= 1, int));
static_assert(TYPE_OF(1.0 <= 1L, int));
static_assert(TYPE_OF(1.0 <= 1L, int));
static_assert(TYPE_OF(1.0 <= 1LL, int));
static_assert(TYPE_OF(1.0 <= 1LL, int));
static_assert(TYPE_OF(1.0 <= 1U, int));
static_assert(TYPE_OF(1.0 <= 1U, int));
static_assert(TYPE_OF(1.0 <= 1UL, int));
static_assert(TYPE_OF(1.0 <= 1UL, int));
static_assert(TYPE_OF(1.0 <= 1ULL, int));
static_assert(TYPE_OF(1.0 <= 1ULL, int));
static_assert(TYPE_OF(1.0 <= 1.0, int));
static_assert(TYPE_OF(1.0 <= 1.0, int));
static_assert(TYPE_OF(1.0 <= 1.0f, int));
static_assert(TYPE_OF(1.0 <= 1.0f, int));
static_assert(TYPE_OF(1.0 <= 1.0L, int));
static_assert(TYPE_OF(1.0 <= 1.0L, int));
static_assert(TYPE_OF(1.0f <= 1, int));
static_assert(TYPE_OF(1.0f <= 1, int));
static_assert(TYPE_OF(1.0f <= 1L, int));
static_assert(TYPE_OF(1.0f <= 1L, int));
static_assert(TYPE_OF(1.0f <= 1LL, int));
static_assert(TYPE_OF(1.0f <= 1LL, int));
static_assert(TYPE_OF(1.0f <= 1U, int));
static_assert(TYPE_OF(1.0f <= 1U, int));
static_assert(TYPE_OF(1.0f <= 1UL, int));
static_assert(TYPE_OF(1.0f <= 1UL, int));
static_assert(TYPE_OF(1.0f <= 1ULL, int));
static_assert(TYPE_OF(1.0f <= 1ULL, int));
static_assert(TYPE_OF(1.0f <= 1.0, int));
static_assert(TYPE_OF(1.0f <= 1.0, int));
static_assert(TYPE_OF(1.0f <= 1.0f, int));
static_assert(TYPE_OF(1.0f <= 1.0f, int));
static_assert(TYPE_OF(1.0f <= 1.0L, int));
static_assert(TYPE_OF(1.0f <= 1.0L, int));
static_assert(TYPE_OF(1.0L <= 1, int));
static_assert(TYPE_OF(1.0L <= 1, int));
static_assert(TYPE_OF(1.0L <= 1L, int));
static_assert(TYPE_OF(1.0L <= 1L, int));
static_assert(TYPE_OF(1.0L <= 1LL, int));
static_assert(TYPE_OF(1.0L <= 1LL, int));
static_assert(TYPE_OF(1.0L <= 1U, int));
static_assert(TYPE_OF(1.0L <= 1U, int));
static_assert(TYPE_OF(1.0L <= 1UL, int));
static_assert(TYPE_OF(1.0L <= 1UL, int));
static_assert(TYPE_OF(1.0L <= 1ULL, int));
static_assert(TYPE_OF(1.0L <= 1ULL, int));
static_assert(TYPE_OF(1.0L <= 1.0, int));
static_assert(TYPE_OF(1.0L <= 1.0, int));
static_assert(TYPE_OF(1.0L <= 1.0f, int));
static_assert(TYPE_OF(1.0L <= 1.0f, int));
static_assert(TYPE_OF(1.0L <= 1.0L, int));
static_assert(TYPE_OF(1.0L <= 1.0L, int));

// Aritmetic operators

static_assert(TYPE_OF(1 + 1, int));
static_assert(TYPE_OF(1 + 1L, long));
static_assert(TYPE_OF(1 + 1LL, long long));
static_assert(TYPE_OF(1 + 1U, unsigned));
static_assert(TYPE_OF(1 + 1UL, unsigned long));
static_assert(TYPE_OF(1 + 1ULL, unsigned long long));
static_assert(TYPE_OF(1 + 1.0, double));
static_assert(TYPE_OF(1 + 1.0f, float));
static_assert(TYPE_OF(1 + 1.0L, long double));
static_assert(TYPE_OF(1L + 1, long));
static_assert(TYPE_OF(1L + 1L, long));
static_assert(TYPE_OF(1L + 1LL, long long));
static_assert(TYPE_OF(1L + 1U, long));
static_assert(TYPE_OF(1L + 1UL, unsigned long));
static_assert(TYPE_OF(1L + 1ULL, unsigned long long));
static_assert(TYPE_OF(1L + 1.0, double));
static_assert(TYPE_OF(1L + 1.0f, float));
static_assert(TYPE_OF(1L + 1.0L, long double));
static_assert(TYPE_OF(1LL + 1, long long));
static_assert(TYPE_OF(1LL + 1L, long long));
static_assert(TYPE_OF(1LL + 1LL, long long));
static_assert(TYPE_OF(1LL + 1U, long long));
static_assert(TYPE_OF(1LL + 1UL, unsigned long long));
static_assert(TYPE_OF(1LL + 1ULL, unsigned long long));
static_assert(TYPE_OF(1LL + 1.0, double));
static_assert(TYPE_OF(1LL + 1.0f, float));
static_assert(TYPE_OF(1LL + 1.0L, long double));
static_assert(TYPE_OF(1U + 1, unsigned));
static_assert(TYPE_OF(1U + 1L, long));
static_assert(TYPE_OF(1U + 1LL, long long));
static_assert(TYPE_OF(1U + 1U, unsigned));
static_assert(TYPE_OF(1U + 1UL, unsigned long));
static_assert(TYPE_OF(1U + 1ULL, unsigned long long));
static_assert(TYPE_OF(1U + 1.0, double));
static_assert(TYPE_OF(1U + 1.0f, float));
static_assert(TYPE_OF(1U + 1.0L, long double));
static_assert(TYPE_OF(1UL + 1, unsigned long));
static_assert(TYPE_OF(1UL + 1L, unsigned long));
static_assert(TYPE_OF(1UL + 1LL, unsigned long long));
static_assert(TYPE_OF(1UL + 1U, unsigned long));
static_assert(TYPE_OF(1UL + 1UL, unsigned long));
static_assert(TYPE_OF(1UL + 1ULL, unsigned long long));
static_assert(TYPE_OF(1UL + 1.0, double));
static_assert(TYPE_OF(1UL + 1.0f, float));
static_assert(TYPE_OF(1UL + 1.0L, long double));
static_assert(TYPE_OF(1ULL + 1, unsigned long long));
static_assert(TYPE_OF(1ULL + 1L, unsigned long long));
static_assert(TYPE_OF(1ULL + 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL + 1U, unsigned long long));
static_assert(TYPE_OF(1ULL + 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL + 1ULL, unsigned long long));
static_assert(TYPE_OF(1ULL + 1.0, double));
static_assert(TYPE_OF(1ULL + 1.0f, float));
static_assert(TYPE_OF(1ULL + 1.0L, long double));
static_assert(TYPE_OF(1.0 + 1, double));
static_assert(TYPE_OF(1.0 + 1L, double));
static_assert(TYPE_OF(1.0 + 1LL, double));
static_assert(TYPE_OF(1.0 + 1U, double));
static_assert(TYPE_OF(1.0 + 1UL, double));
static_assert(TYPE_OF(1.0 + 1ULL, double));
static_assert(TYPE_OF(1.0 + 1.0, double));
static_assert(TYPE_OF(1.0 + 1.0f, double));
static_assert(TYPE_OF(1.0 + 1.0L, long double));
static_assert(TYPE_OF(1.0f + 1, float));
static_assert(TYPE_OF(1.0f + 1L, float));
static_assert(TYPE_OF(1.0f + 1LL, float));
static_assert(TYPE_OF(1.0f + 1U, float));
static_assert(TYPE_OF(1.0f + 1UL, float));
static_assert(TYPE_OF(1.0f + 1ULL, float));
static_assert(TYPE_OF(1.0f + 1.0, double));
static_assert(TYPE_OF(1.0f + 1.0f, float));
static_assert(TYPE_OF(1.0f + 1.0L, long double));
static_assert(TYPE_OF(1.0L + 1, long double));
static_assert(TYPE_OF(1.0L + 1L, long double));
static_assert(TYPE_OF(1.0L + 1LL, long double));
static_assert(TYPE_OF(1.0L + 1U, long double));
static_assert(TYPE_OF(1.0L + 1UL, long double));
static_assert(TYPE_OF(1.0L + 1ULL, long double));
static_assert(TYPE_OF(1.0L + 1.0, long double));
static_assert(TYPE_OF(1.0L + 1.0f, long double));
static_assert(TYPE_OF(1.0L + 1.0L, long double));



static_assert(TYPE_OF(1 - 1, int));
static_assert(TYPE_OF(1 - 1L, long));
static_assert(TYPE_OF(1 - 1LL, long long));
static_assert(TYPE_OF(1 - 1U, unsigned));
static_assert(TYPE_OF(1 - 1UL, unsigned long));
static_assert(TYPE_OF(1 - 1ULL, unsigned long long));
static_assert(TYPE_OF(1 - 1.0, double));
static_assert(TYPE_OF(1 - 1.0f, float));
static_assert(TYPE_OF(1 - 1.0L, long double));
static_assert(TYPE_OF(1L - 1, long));
static_assert(TYPE_OF(1L - 1L, long));
static_assert(TYPE_OF(1L - 1LL, long long));
static_assert(TYPE_OF(1L - 1U, long));
static_assert(TYPE_OF(1L - 1UL, unsigned long));
static_assert(TYPE_OF(1L - 1ULL, unsigned long long));
static_assert(TYPE_OF(1L - 1.0, double));
static_assert(TYPE_OF(1L - 1.0f, float));
static_assert(TYPE_OF(1L - 1.0L, long double));
static_assert(TYPE_OF(1LL - 1, long long));
static_assert(TYPE_OF(1LL - 1L, long long));
static_assert(TYPE_OF(1LL - 1LL, long long));
static_assert(TYPE_OF(1LL - 1U, long long));
static_assert(TYPE_OF(1LL - 1UL, unsigned long long));
static_assert(TYPE_OF(1LL - 1ULL, unsigned long long));
static_assert(TYPE_OF(1LL - 1.0, double));
static_assert(TYPE_OF(1LL - 1.0f, float));
static_assert(TYPE_OF(1LL - 1.0L, long double));
static_assert(TYPE_OF(1U - 1, unsigned));
static_assert(TYPE_OF(1U - 1L, long));
static_assert(TYPE_OF(1U - 1LL, long long));
static_assert(TYPE_OF(1U - 1U, unsigned));
static_assert(TYPE_OF(1U - 1UL, unsigned long));
static_assert(TYPE_OF(1U - 1ULL, unsigned long long));
static_assert(TYPE_OF(1U - 1.0, double));
static_assert(TYPE_OF(1U - 1.0f, float));
static_assert(TYPE_OF(1U - 1.0L, long double));
static_assert(TYPE_OF(1UL - 1, unsigned long));
static_assert(TYPE_OF(1UL - 1L, unsigned long));
static_assert(TYPE_OF(1UL - 1LL, unsigned long long));
static_assert(TYPE_OF(1UL - 1U, unsigned long));
static_assert(TYPE_OF(1UL - 1UL, unsigned long));
static_assert(TYPE_OF(1UL - 1ULL, unsigned long long));
static_assert(TYPE_OF(1UL - 1.0, double));
static_assert(TYPE_OF(1UL - 1.0f, float));
static_assert(TYPE_OF(1UL - 1.0L, long double));
static_assert(TYPE_OF(1ULL - 1, unsigned long long));
static_assert(TYPE_OF(1ULL - 1L, unsigned long long));
static_assert(TYPE_OF(1ULL - 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL - 1U, unsigned long long));
static_assert(TYPE_OF(1ULL - 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL - 1ULL, unsigned long long));
static_assert(TYPE_OF(1ULL - 1.0, double));
static_assert(TYPE_OF(1ULL - 1.0f, float));
static_assert(TYPE_OF(1ULL - 1.0L, long double));
static_assert(TYPE_OF(1.0 - 1, double));
static_assert(TYPE_OF(1.0 - 1L, double));
static_assert(TYPE_OF(1.0 - 1LL, double));
static_assert(TYPE_OF(1.0 - 1U, double));
static_assert(TYPE_OF(1.0 - 1UL, double));
static_assert(TYPE_OF(1.0 - 1ULL, double));
static_assert(TYPE_OF(1.0 - 1.0, double));
static_assert(TYPE_OF(1.0 - 1.0f, double));
static_assert(TYPE_OF(1.0 - 1.0L, long double));
static_assert(TYPE_OF(1.0f - 1, float));
static_assert(TYPE_OF(1.0f - 1L, float));
static_assert(TYPE_OF(1.0f - 1LL, float));
static_assert(TYPE_OF(1.0f - 1U, float));
static_assert(TYPE_OF(1.0f - 1UL, float));
static_assert(TYPE_OF(1.0f - 1ULL, float));
static_assert(TYPE_OF(1.0f - 1.0, double));
static_assert(TYPE_OF(1.0f - 1.0f, float));
static_assert(TYPE_OF(1.0f - 1.0L, long double));
static_assert(TYPE_OF(1.0L - 1, long double));
static_assert(TYPE_OF(1.0L - 1L, long double));
static_assert(TYPE_OF(1.0L - 1LL, long double));
static_assert(TYPE_OF(1.0L - 1U, long double));
static_assert(TYPE_OF(1.0L - 1UL, long double));
static_assert(TYPE_OF(1.0L - 1ULL, long double));
static_assert(TYPE_OF(1.0L - 1.0, long double));
static_assert(TYPE_OF(1.0L - 1.0f, long double));
static_assert(TYPE_OF(1.0L - 1.0L, long double));



static_assert(TYPE_OF(1 * 1, int));
static_assert(TYPE_OF(1 * 1L, long));
static_assert(TYPE_OF(1 * 1LL, long long));
static_assert(TYPE_OF(1 * 1U, unsigned));
static_assert(TYPE_OF(1 * 1UL, unsigned long));
static_assert(TYPE_OF(1 * 1ULL, unsigned long long));
static_assert(TYPE_OF(1 * 1.0, double));
static_assert(TYPE_OF(1 * 1.0f, float));
static_assert(TYPE_OF(1 * 1.0L, long double));
static_assert(TYPE_OF(1L * 1, long));
static_assert(TYPE_OF(1L * 1L, long));
static_assert(TYPE_OF(1L * 1LL, long long));
static_assert(TYPE_OF(1L * 1U, long));
static_assert(TYPE_OF(1L * 1UL, unsigned long));
static_assert(TYPE_OF(1L * 1ULL, unsigned long long));
static_assert(TYPE_OF(1L * 1.0, double));
static_assert(TYPE_OF(1L * 1.0f, float));
static_assert(TYPE_OF(1L * 1.0L, long double));
static_assert(TYPE_OF(1LL * 1, long long));
static_assert(TYPE_OF(1LL * 1L, long long));
static_assert(TYPE_OF(1LL * 1LL, long long));
static_assert(TYPE_OF(1LL * 1U, long long));
static_assert(TYPE_OF(1LL * 1UL, unsigned long long));
static_assert(TYPE_OF(1LL * 1ULL, unsigned long long));
static_assert(TYPE_OF(1LL * 1.0, double));
static_assert(TYPE_OF(1LL * 1.0f, float));
static_assert(TYPE_OF(1LL * 1.0L, long double));
static_assert(TYPE_OF(1U * 1, unsigned));
static_assert(TYPE_OF(1U * 1L, long));
static_assert(TYPE_OF(1U * 1LL, long long));
static_assert(TYPE_OF(1U * 1U, unsigned));
static_assert(TYPE_OF(1U * 1UL, unsigned long));
static_assert(TYPE_OF(1U * 1ULL, unsigned long long));
static_assert(TYPE_OF(1U * 1.0, double));
static_assert(TYPE_OF(1U * 1.0f, float));
static_assert(TYPE_OF(1U * 1.0L, long double));
static_assert(TYPE_OF(1UL * 1, unsigned long));
static_assert(TYPE_OF(1UL * 1L, unsigned long));
static_assert(TYPE_OF(1UL * 1LL, unsigned long long));
static_assert(TYPE_OF(1UL * 1U, unsigned long));
static_assert(TYPE_OF(1UL * 1UL, unsigned long));
static_assert(TYPE_OF(1UL * 1ULL, unsigned long long));
static_assert(TYPE_OF(1UL * 1.0, double));
static_assert(TYPE_OF(1UL * 1.0f, float));
static_assert(TYPE_OF(1UL * 1.0L, long double));
static_assert(TYPE_OF(1ULL * 1, unsigned long long));
static_assert(TYPE_OF(1ULL * 1L, unsigned long long));
static_assert(TYPE_OF(1ULL * 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL * 1U, unsigned long long));
static_assert(TYPE_OF(1ULL * 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL * 1ULL, unsigned long long));
static_assert(TYPE_OF(1ULL * 1.0, double));
static_assert(TYPE_OF(1ULL * 1.0f, float));
static_assert(TYPE_OF(1ULL * 1.0L, long double));
static_assert(TYPE_OF(1.0 * 1, double));
static_assert(TYPE_OF(1.0 * 1L, double));
static_assert(TYPE_OF(1.0 * 1LL, double));
static_assert(TYPE_OF(1.0 * 1U, double));
static_assert(TYPE_OF(1.0 * 1UL, double));
static_assert(TYPE_OF(1.0 * 1ULL, double));
static_assert(TYPE_OF(1.0 * 1.0, double));
static_assert(TYPE_OF(1.0 * 1.0f, double));
static_assert(TYPE_OF(1.0 * 1.0L, long double));
static_assert(TYPE_OF(1.0f * 1, float));
static_assert(TYPE_OF(1.0f * 1L, float));
static_assert(TYPE_OF(1.0f * 1LL, float));
static_assert(TYPE_OF(1.0f * 1U, float));
static_assert(TYPE_OF(1.0f * 1UL, float));
static_assert(TYPE_OF(1.0f * 1ULL, float));
static_assert(TYPE_OF(1.0f * 1.0, double));
static_assert(TYPE_OF(1.0f * 1.0f, float));
static_assert(TYPE_OF(1.0f * 1.0L, long double));
static_assert(TYPE_OF(1.0L * 1, long double));
static_assert(TYPE_OF(1.0L * 1L, long double));
static_assert(TYPE_OF(1.0L * 1LL, long double));
static_assert(TYPE_OF(1.0L * 1U, long double));
static_assert(TYPE_OF(1.0L * 1UL, long double));
static_assert(TYPE_OF(1.0L * 1ULL, long double));
static_assert(TYPE_OF(1.0L * 1.0, long double));
static_assert(TYPE_OF(1.0L * 1.0f, long double));
static_assert(TYPE_OF(1.0L * 1.0L, long double));



static_assert(TYPE_OF(1 / 1, int));
static_assert(TYPE_OF(1 / 1L, long));
static_assert(TYPE_OF(1 / 1LL, long long));
static_assert(TYPE_OF(1 / 1U, unsigned));
static_assert(TYPE_OF(1 / 1UL, unsigned long));
static_assert(TYPE_OF(1 / 1ULL, unsigned long long));
static_assert(TYPE_OF(1 / 1.0, double));
static_assert(TYPE_OF(1 / 1.0f, float));
static_assert(TYPE_OF(1 / 1.0L, long double));
static_assert(TYPE_OF(1L / 1, long));
static_assert(TYPE_OF(1L / 1L, long));
static_assert(TYPE_OF(1L / 1LL, long long));
static_assert(TYPE_OF(1L / 1U, long));
static_assert(TYPE_OF(1L / 1UL, unsigned long));
static_assert(TYPE_OF(1L / 1ULL, unsigned long long));
static_assert(TYPE_OF(1L / 1.0, double));
static_assert(TYPE_OF(1L / 1.0f, float));
static_assert(TYPE_OF(1L / 1.0L, long double));
static_assert(TYPE_OF(1LL / 1, long long));
static_assert(TYPE_OF(1LL / 1L, long long));
static_assert(TYPE_OF(1LL / 1LL, long long));
static_assert(TYPE_OF(1LL / 1U, long long));
static_assert(TYPE_OF(1LL / 1UL, unsigned long long));
static_assert(TYPE_OF(1LL / 1ULL, unsigned long long));
static_assert(TYPE_OF(1LL / 1.0, double));
static_assert(TYPE_OF(1LL / 1.0f, float));
static_assert(TYPE_OF(1LL / 1.0L, long double));
static_assert(TYPE_OF(1U / 1, unsigned));
static_assert(TYPE_OF(1U / 1L, long));
static_assert(TYPE_OF(1U / 1LL, long long));
static_assert(TYPE_OF(1U / 1U, unsigned));
static_assert(TYPE_OF(1U / 1UL, unsigned long));
static_assert(TYPE_OF(1U / 1ULL, unsigned long long));
static_assert(TYPE_OF(1U / 1.0, double));
static_assert(TYPE_OF(1U / 1.0f, float));
static_assert(TYPE_OF(1U / 1.0L, long double));
static_assert(TYPE_OF(1UL / 1, unsigned long));
static_assert(TYPE_OF(1UL / 1L, unsigned long));
static_assert(TYPE_OF(1UL / 1LL, unsigned long long));
static_assert(TYPE_OF(1UL / 1U, unsigned long));
static_assert(TYPE_OF(1UL / 1UL, unsigned long));
static_assert(TYPE_OF(1UL / 1ULL, unsigned long long));
static_assert(TYPE_OF(1UL / 1.0, double));
static_assert(TYPE_OF(1UL / 1.0f, float));
static_assert(TYPE_OF(1UL / 1.0L, long double));
static_assert(TYPE_OF(1ULL / 1, unsigned long long));
static_assert(TYPE_OF(1ULL / 1L, unsigned long long));
static_assert(TYPE_OF(1ULL / 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL / 1U, unsigned long long));
static_assert(TYPE_OF(1ULL / 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL / 1ULL, unsigned long long));
static_assert(TYPE_OF(1ULL / 1.0, double));
static_assert(TYPE_OF(1ULL / 1.0f, float));
static_assert(TYPE_OF(1ULL / 1.0L, long double));
static_assert(TYPE_OF(1.0 / 1, double));
static_assert(TYPE_OF(1.0 / 1L, double));
static_assert(TYPE_OF(1.0 / 1LL, double));
static_assert(TYPE_OF(1.0 / 1U, double));
static_assert(TYPE_OF(1.0 / 1UL, double));
static_assert(TYPE_OF(1.0 / 1ULL, double));
static_assert(TYPE_OF(1.0 / 1.0, double));
static_assert(TYPE_OF(1.0 / 1.0f, double));
static_assert(TYPE_OF(1.0 / 1.0L, long double));
static_assert(TYPE_OF(1.0f / 1, float));
static_assert(TYPE_OF(1.0f / 1L, float));
static_assert(TYPE_OF(1.0f / 1LL, float));
static_assert(TYPE_OF(1.0f / 1U, float));
static_assert(TYPE_OF(1.0f / 1UL, float));
static_assert(TYPE_OF(1.0f / 1ULL, float));
static_assert(TYPE_OF(1.0f / 1.0, double));
static_assert(TYPE_OF(1.0f / 1.0f, float));
static_assert(TYPE_OF(1.0f / 1.0L, long double));
static_assert(TYPE_OF(1.0L / 1, long double));
static_assert(TYPE_OF(1.0L / 1L, long double));
static_assert(TYPE_OF(1.0L / 1LL, long double));
static_assert(TYPE_OF(1.0L / 1U, long double));
static_assert(TYPE_OF(1.0L / 1UL, long double));
static_assert(TYPE_OF(1.0L / 1ULL, long double));
static_assert(TYPE_OF(1.0L / 1.0, long double));
static_assert(TYPE_OF(1.0L / 1.0f, long double));
static_assert(TYPE_OF(1.0L / 1.0L, long double));



static_assert(TYPE_OF(1 % 1, int));
static_assert(TYPE_OF(1 % 1L, long));
static_assert(TYPE_OF(1 % 1LL, long long));
static_assert(TYPE_OF(1 % 1U, unsigned));
static_assert(TYPE_OF(1 % 1UL, unsigned long));
static_assert(TYPE_OF(1 % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1 % 1.0, double));
//static_assert(TYPE_OF(1 % 1.0f, float));
//static_assert(TYPE_OF(1 % 1.0L, long double));
static_assert(TYPE_OF(1L % 1, long));
static_assert(TYPE_OF(1L % 1L, long));
static_assert(TYPE_OF(1L % 1LL, long long));
static_assert(TYPE_OF(1L % 1U, long));
static_assert(TYPE_OF(1L % 1UL, unsigned long));
static_assert(TYPE_OF(1L % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1L % 1.0, double));
//static_assert(TYPE_OF(1L % 1.0f, float));
//static_assert(TYPE_OF(1L % 1.0L, long double));
static_assert(TYPE_OF(1LL % 1, long long));
static_assert(TYPE_OF(1LL % 1L, long long));
static_assert(TYPE_OF(1LL % 1LL, long long));
static_assert(TYPE_OF(1LL % 1U, long long));
static_assert(TYPE_OF(1LL % 1UL, unsigned long long));
static_assert(TYPE_OF(1LL % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1LL % 1.0, double));
//static_assert(TYPE_OF(1LL % 1.0f, float));
//static_assert(TYPE_OF(1LL % 1.0L, long double));
static_assert(TYPE_OF(1U % 1, unsigned));
static_assert(TYPE_OF(1U % 1L, long));
static_assert(TYPE_OF(1U % 1LL, long long));
static_assert(TYPE_OF(1U % 1U, unsigned));
static_assert(TYPE_OF(1U % 1UL, unsigned long));
static_assert(TYPE_OF(1U % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1U % 1.0, double));
//static_assert(TYPE_OF(1U % 1.0f, float));
//static_assert(TYPE_OF(1U % 1.0L, long double));
static_assert(TYPE_OF(1UL % 1, unsigned long));
static_assert(TYPE_OF(1UL % 1L, unsigned long));
static_assert(TYPE_OF(1UL % 1LL, unsigned long long));
static_assert(TYPE_OF(1UL % 1U, unsigned long));
static_assert(TYPE_OF(1UL % 1UL, unsigned long));
static_assert(TYPE_OF(1UL % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1UL % 1.0, double));
//static_assert(TYPE_OF(1UL % 1.0f, float));
//static_assert(TYPE_OF(1UL % 1.0L, long double));
static_assert(TYPE_OF(1ULL % 1, unsigned long long));
static_assert(TYPE_OF(1ULL % 1L, unsigned long long));
static_assert(TYPE_OF(1ULL % 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL % 1U, unsigned long long));
static_assert(TYPE_OF(1ULL % 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1ULL % 1.0, double));
//static_assert(TYPE_OF(1ULL % 1.0f, float));
//static_assert(TYPE_OF(1ULL % 1.0L, long double));
//static_assert(TYPE_OF(1.0 % 1, double));
//static_assert(TYPE_OF(1.0 % 1L, double));
//static_assert(TYPE_OF(1.0 % 1LL, double));
//static_assert(TYPE_OF(1.0 % 1U, double));
//static_assert(TYPE_OF(1.0 % 1UL, double));
//static_assert(TYPE_OF(1.0 % 1ULL, double));
//static_assert(TYPE_OF(1.0 % 1.0, double));
//static_assert(TYPE_OF(1.0 % 1.0f, double));
//static_assert(TYPE_OF(1.0 % 1.0L, long double));
//static_assert(TYPE_OF(1.0f % 1, float));
//static_assert(TYPE_OF(1.0f % 1L, float));
//static_assert(TYPE_OF(1.0f % 1LL, float));
//static_assert(TYPE_OF(1.0f % 1U, float));
//static_assert(TYPE_OF(1.0f % 1UL, float));
//static_assert(TYPE_OF(1.0f % 1ULL, float));
//static_assert(TYPE_OF(1.0f % 1.0, double));
//static_assert(TYPE_OF(1.0f % 1.0f, float));
//static_assert(TYPE_OF(1.0f % 1.0L, long double));
//static_assert(TYPE_OF(1.0L % 1, long double));
//static_assert(TYPE_OF(1.0L % 1L, long double));
//static_assert(TYPE_OF(1.0L % 1LL, long double));
//static_assert(TYPE_OF(1.0L % 1U, long double));
//static_assert(TYPE_OF(1.0L % 1UL, long double));
//static_assert(TYPE_OF(1.0L % 1ULL, long double));
//static_assert(TYPE_OF(1.0L % 1.0, long double));
//static_assert(TYPE_OF(1.0L % 1.0f, long double));
//static_assert(TYPE_OF(1.0L % 1.0L, long double));
#else


#define TYPE_OF(E, T) _Generic(E, T : 1 , default: 0)

//Comparision operators

static_assert(TYPE_OF(1 == 1, int));
static_assert(TYPE_OF(1 == 1, int));
static_assert(TYPE_OF(1 == 1L, int));
static_assert(TYPE_OF(1 == 1L, int));
static_assert(TYPE_OF(1 == 1LL, int));
static_assert(TYPE_OF(1 == 1LL, int));
static_assert(TYPE_OF(1 == 1U, int));
static_assert(TYPE_OF(1 == 1U, int));
static_assert(TYPE_OF(1 == 1UL, int));
static_assert(TYPE_OF(1 == 1UL, int));
static_assert(TYPE_OF(1 == 1ULL, int));
static_assert(TYPE_OF(1 == 1ULL, int));
static_assert(TYPE_OF(1 == 1.0, int));
static_assert(TYPE_OF(1 == 1.0, int));
static_assert(TYPE_OF(1 == 1.0f, int));
static_assert(TYPE_OF(1 == 1.0f, int));
static_assert(TYPE_OF(1 == 1.0L, int));
static_assert(TYPE_OF(1 == 1.0L, int));
static_assert(TYPE_OF(1L == 1, int));
static_assert(TYPE_OF(1L == 1, int));
static_assert(TYPE_OF(1L == 1L, int));
static_assert(TYPE_OF(1L == 1L, int));
static_assert(TYPE_OF(1L == 1LL, int));
static_assert(TYPE_OF(1L == 1LL, int));
static_assert(TYPE_OF(1L == 1U, int));
static_assert(TYPE_OF(1L == 1U, int));
static_assert(TYPE_OF(1L == 1UL, int));
static_assert(TYPE_OF(1L == 1UL, int));
static_assert(TYPE_OF(1L == 1ULL, int));
static_assert(TYPE_OF(1L == 1ULL, int));
static_assert(TYPE_OF(1L == 1.0, int));
static_assert(TYPE_OF(1L == 1.0, int));
static_assert(TYPE_OF(1L == 1.0f, int));
static_assert(TYPE_OF(1L == 1.0f, int));
static_assert(TYPE_OF(1L == 1.0L, int));
static_assert(TYPE_OF(1L == 1.0L, int));
static_assert(TYPE_OF(1LL == 1, int));
static_assert(TYPE_OF(1LL == 1, int));
static_assert(TYPE_OF(1LL == 1L, int));
static_assert(TYPE_OF(1LL == 1L, int));
static_assert(TYPE_OF(1LL == 1LL, int));
static_assert(TYPE_OF(1LL == 1LL, int));
static_assert(TYPE_OF(1LL == 1U, int));
static_assert(TYPE_OF(1LL == 1U, int));
static_assert(TYPE_OF(1LL == 1UL, int));
static_assert(TYPE_OF(1LL == 1UL, int));
static_assert(TYPE_OF(1LL == 1ULL, int));
static_assert(TYPE_OF(1LL == 1ULL, int));
static_assert(TYPE_OF(1LL == 1.0, int));
static_assert(TYPE_OF(1LL == 1.0, int));
static_assert(TYPE_OF(1LL == 1.0f, int));
static_assert(TYPE_OF(1LL == 1.0f, int));
static_assert(TYPE_OF(1LL == 1.0L, int));
static_assert(TYPE_OF(1LL == 1.0L, int));
static_assert(TYPE_OF(1U == 1, int));
static_assert(TYPE_OF(1U == 1, int));
static_assert(TYPE_OF(1U == 1L, int));
static_assert(TYPE_OF(1U == 1L, int));
static_assert(TYPE_OF(1U == 1LL, int));
static_assert(TYPE_OF(1U == 1LL, int));
static_assert(TYPE_OF(1U == 1U, int));
static_assert(TYPE_OF(1U == 1U, int));
static_assert(TYPE_OF(1U == 1UL, int));
static_assert(TYPE_OF(1U == 1UL, int));
static_assert(TYPE_OF(1U == 1ULL, int));
static_assert(TYPE_OF(1U == 1ULL, int));
static_assert(TYPE_OF(1U == 1.0, int));
static_assert(TYPE_OF(1U == 1.0, int));
static_assert(TYPE_OF(1U == 1.0f, int));
static_assert(TYPE_OF(1U == 1.0f, int));
static_assert(TYPE_OF(1U == 1.0L, int));
static_assert(TYPE_OF(1U == 1.0L, int));
static_assert(TYPE_OF(1UL == 1, int));
static_assert(TYPE_OF(1UL == 1, int));
static_assert(TYPE_OF(1UL == 1L, int));
static_assert(TYPE_OF(1UL == 1L, int));
static_assert(TYPE_OF(1UL == 1LL, int));
static_assert(TYPE_OF(1UL == 1LL, int));
static_assert(TYPE_OF(1UL == 1U, int));
static_assert(TYPE_OF(1UL == 1U, int));
static_assert(TYPE_OF(1UL == 1UL, int));
static_assert(TYPE_OF(1UL == 1UL, int));
static_assert(TYPE_OF(1UL == 1ULL, int));
static_assert(TYPE_OF(1UL == 1ULL, int));
static_assert(TYPE_OF(1UL == 1.0, int));
static_assert(TYPE_OF(1UL == 1.0, int));
static_assert(TYPE_OF(1UL == 1.0f, int));
static_assert(TYPE_OF(1UL == 1.0f, int));
static_assert(TYPE_OF(1UL == 1.0L, int));
static_assert(TYPE_OF(1UL == 1.0L, int));
static_assert(TYPE_OF(1ULL == 1, int));
static_assert(TYPE_OF(1ULL == 1, int));
static_assert(TYPE_OF(1ULL == 1L, int));
static_assert(TYPE_OF(1ULL == 1L, int));
static_assert(TYPE_OF(1ULL == 1LL, int));
static_assert(TYPE_OF(1ULL == 1LL, int));
static_assert(TYPE_OF(1ULL == 1U, int));
static_assert(TYPE_OF(1ULL == 1U, int));
static_assert(TYPE_OF(1ULL == 1UL, int));
static_assert(TYPE_OF(1ULL == 1UL, int));
static_assert(TYPE_OF(1ULL == 1ULL, int));
static_assert(TYPE_OF(1ULL == 1ULL, int));
static_assert(TYPE_OF(1ULL == 1.0, int));
static_assert(TYPE_OF(1ULL == 1.0, int));
static_assert(TYPE_OF(1ULL == 1.0f, int));
static_assert(TYPE_OF(1ULL == 1.0f, int));
static_assert(TYPE_OF(1ULL == 1.0L, int));
static_assert(TYPE_OF(1ULL == 1.0L, int));
static_assert(TYPE_OF(1.0 == 1, int));
static_assert(TYPE_OF(1.0 == 1, int));
static_assert(TYPE_OF(1.0 == 1L, int));
static_assert(TYPE_OF(1.0 == 1L, int));
static_assert(TYPE_OF(1.0 == 1LL, int));
static_assert(TYPE_OF(1.0 == 1LL, int));
static_assert(TYPE_OF(1.0 == 1U, int));
static_assert(TYPE_OF(1.0 == 1U, int));
static_assert(TYPE_OF(1.0 == 1UL, int));
static_assert(TYPE_OF(1.0 == 1UL, int));
static_assert(TYPE_OF(1.0 == 1ULL, int));
static_assert(TYPE_OF(1.0 == 1ULL, int));
static_assert(TYPE_OF(1.0 == 1.0, int));
static_assert(TYPE_OF(1.0 == 1.0, int));
static_assert(TYPE_OF(1.0 == 1.0f, int));
static_assert(TYPE_OF(1.0 == 1.0f, int));
static_assert(TYPE_OF(1.0 == 1.0L, int));
static_assert(TYPE_OF(1.0 == 1.0L, int));
static_assert(TYPE_OF(1.0f == 1, int));
static_assert(TYPE_OF(1.0f == 1, int));
static_assert(TYPE_OF(1.0f == 1L, int));
static_assert(TYPE_OF(1.0f == 1L, int));
static_assert(TYPE_OF(1.0f == 1LL, int));
static_assert(TYPE_OF(1.0f == 1LL, int));
static_assert(TYPE_OF(1.0f == 1U, int));
static_assert(TYPE_OF(1.0f == 1U, int));
static_assert(TYPE_OF(1.0f == 1UL, int));
static_assert(TYPE_OF(1.0f == 1UL, int));
static_assert(TYPE_OF(1.0f == 1ULL, int));
static_assert(TYPE_OF(1.0f == 1ULL, int));
static_assert(TYPE_OF(1.0f == 1.0, int));
static_assert(TYPE_OF(1.0f == 1.0, int));
static_assert(TYPE_OF(1.0f == 1.0f, int));
static_assert(TYPE_OF(1.0f == 1.0f, int));
static_assert(TYPE_OF(1.0f == 1.0L, int));
static_assert(TYPE_OF(1.0f == 1.0L, int));
static_assert(TYPE_OF(1.0L == 1, int));
static_assert(TYPE_OF(1.0L == 1, int));
static_assert(TYPE_OF(1.0L == 1L, int));
static_assert(TYPE_OF(1.0L == 1L, int));
static_assert(TYPE_OF(1.0L == 1LL, int));
static_assert(TYPE_OF(1.0L == 1LL, int));
static_assert(TYPE_OF(1.0L == 1U, int));
static_assert(TYPE_OF(1.0L == 1U, int));
static_assert(TYPE_OF(1.0L == 1UL, int));
static_assert(TYPE_OF(1.0L == 1UL, int));
static_assert(TYPE_OF(1.0L == 1ULL, int));
static_assert(TYPE_OF(1.0L == 1ULL, int));
static_assert(TYPE_OF(1.0L == 1.0, int));
static_assert(TYPE_OF(1.0L == 1.0, int));
static_assert(TYPE_OF(1.0L == 1.0f, int));
static_assert(TYPE_OF(1.0L == 1.0f, int));
static_assert(TYPE_OF(1.0L == 1.0L, int));
static_assert(TYPE_OF(1.0L == 1.0L, int));
static_assert(TYPE_OF(1 != 1, int));
static_assert(TYPE_OF(1 != 1, int));
static_assert(TYPE_OF(1 != 1L, int));
static_assert(TYPE_OF(1 != 1L, int));
static_assert(TYPE_OF(1 != 1LL, int));
static_assert(TYPE_OF(1 != 1LL, int));
static_assert(TYPE_OF(1 != 1U, int));
static_assert(TYPE_OF(1 != 1U, int));
static_assert(TYPE_OF(1 != 1UL, int));
static_assert(TYPE_OF(1 != 1UL, int));
static_assert(TYPE_OF(1 != 1ULL, int));
static_assert(TYPE_OF(1 != 1ULL, int));
static_assert(TYPE_OF(1 != 1.0, int));
static_assert(TYPE_OF(1 != 1.0, int));
static_assert(TYPE_OF(1 != 1.0f, int));
static_assert(TYPE_OF(1 != 1.0f, int));
static_assert(TYPE_OF(1 != 1.0L, int));
static_assert(TYPE_OF(1 != 1.0L, int));
static_assert(TYPE_OF(1L != 1, int));
static_assert(TYPE_OF(1L != 1, int));
static_assert(TYPE_OF(1L != 1L, int));
static_assert(TYPE_OF(1L != 1L, int));
static_assert(TYPE_OF(1L != 1LL, int));
static_assert(TYPE_OF(1L != 1LL, int));
static_assert(TYPE_OF(1L != 1U, int));
static_assert(TYPE_OF(1L != 1U, int));
static_assert(TYPE_OF(1L != 1UL, int));
static_assert(TYPE_OF(1L != 1UL, int));
static_assert(TYPE_OF(1L != 1ULL, int));
static_assert(TYPE_OF(1L != 1ULL, int));
static_assert(TYPE_OF(1L != 1.0, int));
static_assert(TYPE_OF(1L != 1.0, int));
static_assert(TYPE_OF(1L != 1.0f, int));
static_assert(TYPE_OF(1L != 1.0f, int));
static_assert(TYPE_OF(1L != 1.0L, int));
static_assert(TYPE_OF(1L != 1.0L, int));
static_assert(TYPE_OF(1LL != 1, int));
static_assert(TYPE_OF(1LL != 1, int));
static_assert(TYPE_OF(1LL != 1L, int));
static_assert(TYPE_OF(1LL != 1L, int));
static_assert(TYPE_OF(1LL != 1LL, int));
static_assert(TYPE_OF(1LL != 1LL, int));
static_assert(TYPE_OF(1LL != 1U, int));
static_assert(TYPE_OF(1LL != 1U, int));
static_assert(TYPE_OF(1LL != 1UL, int));
static_assert(TYPE_OF(1LL != 1UL, int));
static_assert(TYPE_OF(1LL != 1ULL, int));
static_assert(TYPE_OF(1LL != 1ULL, int));
static_assert(TYPE_OF(1LL != 1.0, int));
static_assert(TYPE_OF(1LL != 1.0, int));
static_assert(TYPE_OF(1LL != 1.0f, int));
static_assert(TYPE_OF(1LL != 1.0f, int));
static_assert(TYPE_OF(1LL != 1.0L, int));
static_assert(TYPE_OF(1LL != 1.0L, int));
static_assert(TYPE_OF(1U != 1, int));
static_assert(TYPE_OF(1U != 1, int));
static_assert(TYPE_OF(1U != 1L, int));
static_assert(TYPE_OF(1U != 1L, int));
static_assert(TYPE_OF(1U != 1LL, int));
static_assert(TYPE_OF(1U != 1LL, int));
static_assert(TYPE_OF(1U != 1U, int));
static_assert(TYPE_OF(1U != 1U, int));
static_assert(TYPE_OF(1U != 1UL, int));
static_assert(TYPE_OF(1U != 1UL, int));
static_assert(TYPE_OF(1U != 1ULL, int));
static_assert(TYPE_OF(1U != 1ULL, int));
static_assert(TYPE_OF(1U != 1.0, int));
static_assert(TYPE_OF(1U != 1.0, int));
static_assert(TYPE_OF(1U != 1.0f, int));
static_assert(TYPE_OF(1U != 1.0f, int));
static_assert(TYPE_OF(1U != 1.0L, int));
static_assert(TYPE_OF(1U != 1.0L, int));
static_assert(TYPE_OF(1UL != 1, int));
static_assert(TYPE_OF(1UL != 1, int));
static_assert(TYPE_OF(1UL != 1L, int));
static_assert(TYPE_OF(1UL != 1L, int));
static_assert(TYPE_OF(1UL != 1LL, int));
static_assert(TYPE_OF(1UL != 1LL, int));
static_assert(TYPE_OF(1UL != 1U, int));
static_assert(TYPE_OF(1UL != 1U, int));
static_assert(TYPE_OF(1UL != 1UL, int));
static_assert(TYPE_OF(1UL != 1UL, int));
static_assert(TYPE_OF(1UL != 1ULL, int));
static_assert(TYPE_OF(1UL != 1ULL, int));
static_assert(TYPE_OF(1UL != 1.0, int));
static_assert(TYPE_OF(1UL != 1.0, int));
static_assert(TYPE_OF(1UL != 1.0f, int));
static_assert(TYPE_OF(1UL != 1.0f, int));
static_assert(TYPE_OF(1UL != 1.0L, int));
static_assert(TYPE_OF(1UL != 1.0L, int));
static_assert(TYPE_OF(1ULL != 1, int));
static_assert(TYPE_OF(1ULL != 1, int));
static_assert(TYPE_OF(1ULL != 1L, int));
static_assert(TYPE_OF(1ULL != 1L, int));
static_assert(TYPE_OF(1ULL != 1LL, int));
static_assert(TYPE_OF(1ULL != 1LL, int));
static_assert(TYPE_OF(1ULL != 1U, int));
static_assert(TYPE_OF(1ULL != 1U, int));
static_assert(TYPE_OF(1ULL != 1UL, int));
static_assert(TYPE_OF(1ULL != 1UL, int));
static_assert(TYPE_OF(1ULL != 1ULL, int));
static_assert(TYPE_OF(1ULL != 1ULL, int));
static_assert(TYPE_OF(1ULL != 1.0, int));
static_assert(TYPE_OF(1ULL != 1.0, int));
static_assert(TYPE_OF(1ULL != 1.0f, int));
static_assert(TYPE_OF(1ULL != 1.0f, int));
static_assert(TYPE_OF(1ULL != 1.0L, int));
static_assert(TYPE_OF(1ULL != 1.0L, int));
static_assert(TYPE_OF(1.0 != 1, int));
static_assert(TYPE_OF(1.0 != 1, int));
static_assert(TYPE_OF(1.0 != 1L, int));
static_assert(TYPE_OF(1.0 != 1L, int));
static_assert(TYPE_OF(1.0 != 1LL, int));
static_assert(TYPE_OF(1.0 != 1LL, int));
static_assert(TYPE_OF(1.0 != 1U, int));
static_assert(TYPE_OF(1.0 != 1U, int));
static_assert(TYPE_OF(1.0 != 1UL, int));
static_assert(TYPE_OF(1.0 != 1UL, int));
static_assert(TYPE_OF(1.0 != 1ULL, int));
static_assert(TYPE_OF(1.0 != 1ULL, int));
static_assert(TYPE_OF(1.0 != 1.0, int));
static_assert(TYPE_OF(1.0 != 1.0, int));
static_assert(TYPE_OF(1.0 != 1.0f, int));
static_assert(TYPE_OF(1.0 != 1.0f, int));
static_assert(TYPE_OF(1.0 != 1.0L, int));
static_assert(TYPE_OF(1.0 != 1.0L, int));
static_assert(TYPE_OF(1.0f != 1, int));
static_assert(TYPE_OF(1.0f != 1, int));
static_assert(TYPE_OF(1.0f != 1L, int));
static_assert(TYPE_OF(1.0f != 1L, int));
static_assert(TYPE_OF(1.0f != 1LL, int));
static_assert(TYPE_OF(1.0f != 1LL, int));
static_assert(TYPE_OF(1.0f != 1U, int));
static_assert(TYPE_OF(1.0f != 1U, int));
static_assert(TYPE_OF(1.0f != 1UL, int));
static_assert(TYPE_OF(1.0f != 1UL, int));
static_assert(TYPE_OF(1.0f != 1ULL, int));
static_assert(TYPE_OF(1.0f != 1ULL, int));
static_assert(TYPE_OF(1.0f != 1.0, int));
static_assert(TYPE_OF(1.0f != 1.0, int));
static_assert(TYPE_OF(1.0f != 1.0f, int));
static_assert(TYPE_OF(1.0f != 1.0f, int));
static_assert(TYPE_OF(1.0f != 1.0L, int));
static_assert(TYPE_OF(1.0f != 1.0L, int));
static_assert(TYPE_OF(1.0L != 1, int));
static_assert(TYPE_OF(1.0L != 1, int));
static_assert(TYPE_OF(1.0L != 1L, int));
static_assert(TYPE_OF(1.0L != 1L, int));
static_assert(TYPE_OF(1.0L != 1LL, int));
static_assert(TYPE_OF(1.0L != 1LL, int));
static_assert(TYPE_OF(1.0L != 1U, int));
static_assert(TYPE_OF(1.0L != 1U, int));
static_assert(TYPE_OF(1.0L != 1UL, int));
static_assert(TYPE_OF(1.0L != 1UL, int));
static_assert(TYPE_OF(1.0L != 1ULL, int));
static_assert(TYPE_OF(1.0L != 1ULL, int));
static_assert(TYPE_OF(1.0L != 1.0, int));
static_assert(TYPE_OF(1.0L != 1.0, int));
static_assert(TYPE_OF(1.0L != 1.0f, int));
static_assert(TYPE_OF(1.0L != 1.0f, int));
static_assert(TYPE_OF(1.0L != 1.0L, int));
static_assert(TYPE_OF(1.0L != 1.0L, int));

//Relational operators

static_assert(TYPE_OF(1 > 1, int));
static_assert(TYPE_OF(1 > 1, int));
static_assert(TYPE_OF(1 > 1L, int));
static_assert(TYPE_OF(1 > 1L, int));
static_assert(TYPE_OF(1 > 1LL, int));
static_assert(TYPE_OF(1 > 1LL, int));
static_assert(TYPE_OF(1 > 1U, int));
static_assert(TYPE_OF(1 > 1U, int));
static_assert(TYPE_OF(1 > 1UL, int));
static_assert(TYPE_OF(1 > 1UL, int));
static_assert(TYPE_OF(1 > 1ULL, int));
static_assert(TYPE_OF(1 > 1ULL, int));
static_assert(TYPE_OF(1 > 1.0, int));
static_assert(TYPE_OF(1 > 1.0, int));
static_assert(TYPE_OF(1 > 1.0f, int));
static_assert(TYPE_OF(1 > 1.0f, int));
static_assert(TYPE_OF(1 > 1.0L, int));
static_assert(TYPE_OF(1 > 1.0L, int));
static_assert(TYPE_OF(1L > 1, int));
static_assert(TYPE_OF(1L > 1, int));
static_assert(TYPE_OF(1L > 1L, int));
static_assert(TYPE_OF(1L > 1L, int));
static_assert(TYPE_OF(1L > 1LL, int));
static_assert(TYPE_OF(1L > 1LL, int));
static_assert(TYPE_OF(1L > 1U, int));
static_assert(TYPE_OF(1L > 1U, int));
static_assert(TYPE_OF(1L > 1UL, int));
static_assert(TYPE_OF(1L > 1UL, int));
static_assert(TYPE_OF(1L > 1ULL, int));
static_assert(TYPE_OF(1L > 1ULL, int));
static_assert(TYPE_OF(1L > 1.0, int));
static_assert(TYPE_OF(1L > 1.0, int));
static_assert(TYPE_OF(1L > 1.0f, int));
static_assert(TYPE_OF(1L > 1.0f, int));
static_assert(TYPE_OF(1L > 1.0L, int));
static_assert(TYPE_OF(1L > 1.0L, int));
static_assert(TYPE_OF(1LL > 1, int));
static_assert(TYPE_OF(1LL > 1, int));
static_assert(TYPE_OF(1LL > 1L, int));
static_assert(TYPE_OF(1LL > 1L, int));
static_assert(TYPE_OF(1LL > 1LL, int));
static_assert(TYPE_OF(1LL > 1LL, int));
static_assert(TYPE_OF(1LL > 1U, int));
static_assert(TYPE_OF(1LL > 1U, int));
static_assert(TYPE_OF(1LL > 1UL, int));
static_assert(TYPE_OF(1LL > 1UL, int));
static_assert(TYPE_OF(1LL > 1ULL, int));
static_assert(TYPE_OF(1LL > 1ULL, int));
static_assert(TYPE_OF(1LL > 1.0, int));
static_assert(TYPE_OF(1LL > 1.0, int));
static_assert(TYPE_OF(1LL > 1.0f, int));
static_assert(TYPE_OF(1LL > 1.0f, int));
static_assert(TYPE_OF(1LL > 1.0L, int));
static_assert(TYPE_OF(1LL > 1.0L, int));
static_assert(TYPE_OF(1U > 1, int));
static_assert(TYPE_OF(1U > 1, int));
static_assert(TYPE_OF(1U > 1L, int));
static_assert(TYPE_OF(1U > 1L, int));
static_assert(TYPE_OF(1U > 1LL, int));
static_assert(TYPE_OF(1U > 1LL, int));
static_assert(TYPE_OF(1U > 1U, int));
static_assert(TYPE_OF(1U > 1U, int));
static_assert(TYPE_OF(1U > 1UL, int));
static_assert(TYPE_OF(1U > 1UL, int));
static_assert(TYPE_OF(1U > 1ULL, int));
static_assert(TYPE_OF(1U > 1ULL, int));
static_assert(TYPE_OF(1U > 1.0, int));
static_assert(TYPE_OF(1U > 1.0, int));
static_assert(TYPE_OF(1U > 1.0f, int));
static_assert(TYPE_OF(1U > 1.0f, int));
static_assert(TYPE_OF(1U > 1.0L, int));
static_assert(TYPE_OF(1U > 1.0L, int));
static_assert(TYPE_OF(1UL > 1, int));
static_assert(TYPE_OF(1UL > 1, int));
static_assert(TYPE_OF(1UL > 1L, int));
static_assert(TYPE_OF(1UL > 1L, int));
static_assert(TYPE_OF(1UL > 1LL, int));
static_assert(TYPE_OF(1UL > 1LL, int));
static_assert(TYPE_OF(1UL > 1U, int));
static_assert(TYPE_OF(1UL > 1U, int));
static_assert(TYPE_OF(1UL > 1UL, int));
static_assert(TYPE_OF(1UL > 1UL, int));
static_assert(TYPE_OF(1UL > 1ULL, int));
static_assert(TYPE_OF(1UL > 1ULL, int));
static_assert(TYPE_OF(1UL > 1.0, int));
static_assert(TYPE_OF(1UL > 1.0, int));
static_assert(TYPE_OF(1UL > 1.0f, int));
static_assert(TYPE_OF(1UL > 1.0f, int));
static_assert(TYPE_OF(1UL > 1.0L, int));
static_assert(TYPE_OF(1UL > 1.0L, int));
static_assert(TYPE_OF(1ULL > 1, int));
static_assert(TYPE_OF(1ULL > 1, int));
static_assert(TYPE_OF(1ULL > 1L, int));
static_assert(TYPE_OF(1ULL > 1L, int));
static_assert(TYPE_OF(1ULL > 1LL, int));
static_assert(TYPE_OF(1ULL > 1LL, int));
static_assert(TYPE_OF(1ULL > 1U, int));
static_assert(TYPE_OF(1ULL > 1U, int));
static_assert(TYPE_OF(1ULL > 1UL, int));
static_assert(TYPE_OF(1ULL > 1UL, int));
static_assert(TYPE_OF(1ULL > 1ULL, int));
static_assert(TYPE_OF(1ULL > 1ULL, int));
static_assert(TYPE_OF(1ULL > 1.0, int));
static_assert(TYPE_OF(1ULL > 1.0, int));
static_assert(TYPE_OF(1ULL > 1.0f, int));
static_assert(TYPE_OF(1ULL > 1.0f, int));
static_assert(TYPE_OF(1ULL > 1.0L, int));
static_assert(TYPE_OF(1ULL > 1.0L, int));
static_assert(TYPE_OF(1.0 > 1, int));
static_assert(TYPE_OF(1.0 > 1, int));
static_assert(TYPE_OF(1.0 > 1L, int));
static_assert(TYPE_OF(1.0 > 1L, int));
static_assert(TYPE_OF(1.0 > 1LL, int));
static_assert(TYPE_OF(1.0 > 1LL, int));
static_assert(TYPE_OF(1.0 > 1U, int));
static_assert(TYPE_OF(1.0 > 1U, int));
static_assert(TYPE_OF(1.0 > 1UL, int));
static_assert(TYPE_OF(1.0 > 1UL, int));
static_assert(TYPE_OF(1.0 > 1ULL, int));
static_assert(TYPE_OF(1.0 > 1ULL, int));
static_assert(TYPE_OF(1.0 > 1.0, int));
static_assert(TYPE_OF(1.0 > 1.0, int));
static_assert(TYPE_OF(1.0 > 1.0f, int));
static_assert(TYPE_OF(1.0 > 1.0f, int));
static_assert(TYPE_OF(1.0 > 1.0L, int));
static_assert(TYPE_OF(1.0 > 1.0L, int));
static_assert(TYPE_OF(1.0f > 1, int));
static_assert(TYPE_OF(1.0f > 1, int));
static_assert(TYPE_OF(1.0f > 1L, int));
static_assert(TYPE_OF(1.0f > 1L, int));
static_assert(TYPE_OF(1.0f > 1LL, int));
static_assert(TYPE_OF(1.0f > 1LL, int));
static_assert(TYPE_OF(1.0f > 1U, int));
static_assert(TYPE_OF(1.0f > 1U, int));
static_assert(TYPE_OF(1.0f > 1UL, int));
static_assert(TYPE_OF(1.0f > 1UL, int));
static_assert(TYPE_OF(1.0f > 1ULL, int));
static_assert(TYPE_OF(1.0f > 1ULL, int));
static_assert(TYPE_OF(1.0f > 1.0, int));
static_assert(TYPE_OF(1.0f > 1.0, int));
static_assert(TYPE_OF(1.0f > 1.0f, int));
static_assert(TYPE_OF(1.0f > 1.0f, int));
static_assert(TYPE_OF(1.0f > 1.0L, int));
static_assert(TYPE_OF(1.0f > 1.0L, int));
static_assert(TYPE_OF(1.0L > 1, int));
static_assert(TYPE_OF(1.0L > 1, int));
static_assert(TYPE_OF(1.0L > 1L, int));
static_assert(TYPE_OF(1.0L > 1L, int));
static_assert(TYPE_OF(1.0L > 1LL, int));
static_assert(TYPE_OF(1.0L > 1LL, int));
static_assert(TYPE_OF(1.0L > 1U, int));
static_assert(TYPE_OF(1.0L > 1U, int));
static_assert(TYPE_OF(1.0L > 1UL, int));
static_assert(TYPE_OF(1.0L > 1UL, int));
static_assert(TYPE_OF(1.0L > 1ULL, int));
static_assert(TYPE_OF(1.0L > 1ULL, int));
static_assert(TYPE_OF(1.0L > 1.0, int));
static_assert(TYPE_OF(1.0L > 1.0, int));
static_assert(TYPE_OF(1.0L > 1.0f, int));
static_assert(TYPE_OF(1.0L > 1.0f, int));
static_assert(TYPE_OF(1.0L > 1.0L, int));
static_assert(TYPE_OF(1.0L > 1.0L, int));
static_assert(TYPE_OF(1 >= 1, int));
static_assert(TYPE_OF(1 >= 1, int));
static_assert(TYPE_OF(1 >= 1L, int));
static_assert(TYPE_OF(1 >= 1L, int));
static_assert(TYPE_OF(1 >= 1LL, int));
static_assert(TYPE_OF(1 >= 1LL, int));
static_assert(TYPE_OF(1 >= 1U, int));
static_assert(TYPE_OF(1 >= 1U, int));
static_assert(TYPE_OF(1 >= 1UL, int));
static_assert(TYPE_OF(1 >= 1UL, int));
static_assert(TYPE_OF(1 >= 1ULL, int));
static_assert(TYPE_OF(1 >= 1ULL, int));
static_assert(TYPE_OF(1 >= 1.0, int));
static_assert(TYPE_OF(1 >= 1.0, int));
static_assert(TYPE_OF(1 >= 1.0f, int));
static_assert(TYPE_OF(1 >= 1.0f, int));
static_assert(TYPE_OF(1 >= 1.0L, int));
static_assert(TYPE_OF(1 >= 1.0L, int));
static_assert(TYPE_OF(1L >= 1, int));
static_assert(TYPE_OF(1L >= 1, int));
static_assert(TYPE_OF(1L >= 1L, int));
static_assert(TYPE_OF(1L >= 1L, int));
static_assert(TYPE_OF(1L >= 1LL, int));
static_assert(TYPE_OF(1L >= 1LL, int));
static_assert(TYPE_OF(1L >= 1U, int));
static_assert(TYPE_OF(1L >= 1U, int));
static_assert(TYPE_OF(1L >= 1UL, int));
static_assert(TYPE_OF(1L >= 1UL, int));
static_assert(TYPE_OF(1L >= 1ULL, int));
static_assert(TYPE_OF(1L >= 1ULL, int));
static_assert(TYPE_OF(1L >= 1.0, int));
static_assert(TYPE_OF(1L >= 1.0, int));
static_assert(TYPE_OF(1L >= 1.0f, int));
static_assert(TYPE_OF(1L >= 1.0f, int));
static_assert(TYPE_OF(1L >= 1.0L, int));
static_assert(TYPE_OF(1L >= 1.0L, int));
static_assert(TYPE_OF(1LL >= 1, int));
static_assert(TYPE_OF(1LL >= 1, int));
static_assert(TYPE_OF(1LL >= 1L, int));
static_assert(TYPE_OF(1LL >= 1L, int));
static_assert(TYPE_OF(1LL >= 1LL, int));
static_assert(TYPE_OF(1LL >= 1LL, int));
static_assert(TYPE_OF(1LL >= 1U, int));
static_assert(TYPE_OF(1LL >= 1U, int));
static_assert(TYPE_OF(1LL >= 1UL, int));
static_assert(TYPE_OF(1LL >= 1UL, int));
static_assert(TYPE_OF(1LL >= 1ULL, int));
static_assert(TYPE_OF(1LL >= 1ULL, int));
static_assert(TYPE_OF(1LL >= 1.0, int));
static_assert(TYPE_OF(1LL >= 1.0, int));
static_assert(TYPE_OF(1LL >= 1.0f, int));
static_assert(TYPE_OF(1LL >= 1.0f, int));
static_assert(TYPE_OF(1LL >= 1.0L, int));
static_assert(TYPE_OF(1LL >= 1.0L, int));
static_assert(TYPE_OF(1U >= 1, int));
static_assert(TYPE_OF(1U >= 1, int));
static_assert(TYPE_OF(1U >= 1L, int));
static_assert(TYPE_OF(1U >= 1L, int));
static_assert(TYPE_OF(1U >= 1LL, int));
static_assert(TYPE_OF(1U >= 1LL, int));
static_assert(TYPE_OF(1U >= 1U, int));
static_assert(TYPE_OF(1U >= 1U, int));
static_assert(TYPE_OF(1U >= 1UL, int));
static_assert(TYPE_OF(1U >= 1UL, int));
static_assert(TYPE_OF(1U >= 1ULL, int));
static_assert(TYPE_OF(1U >= 1ULL, int));
static_assert(TYPE_OF(1U >= 1.0, int));
static_assert(TYPE_OF(1U >= 1.0, int));
static_assert(TYPE_OF(1U >= 1.0f, int));
static_assert(TYPE_OF(1U >= 1.0f, int));
static_assert(TYPE_OF(1U >= 1.0L, int));
static_assert(TYPE_OF(1U >= 1.0L, int));
static_assert(TYPE_OF(1UL >= 1, int));
static_assert(TYPE_OF(1UL >= 1, int));
static_assert(TYPE_OF(1UL >= 1L, int));
static_assert(TYPE_OF(1UL >= 1L, int));
static_assert(TYPE_OF(1UL >= 1LL, int));
static_assert(TYPE_OF(1UL >= 1LL, int));
static_assert(TYPE_OF(1UL >= 1U, int));
static_assert(TYPE_OF(1UL >= 1U, int));
static_assert(TYPE_OF(1UL >= 1UL, int));
static_assert(TYPE_OF(1UL >= 1UL, int));
static_assert(TYPE_OF(1UL >= 1ULL, int));
static_assert(TYPE_OF(1UL >= 1ULL, int));
static_assert(TYPE_OF(1UL >= 1.0, int));
static_assert(TYPE_OF(1UL >= 1.0, int));
static_assert(TYPE_OF(1UL >= 1.0f, int));
static_assert(TYPE_OF(1UL >= 1.0f, int));
static_assert(TYPE_OF(1UL >= 1.0L, int));
static_assert(TYPE_OF(1UL >= 1.0L, int));
static_assert(TYPE_OF(1ULL >= 1, int));
static_assert(TYPE_OF(1ULL >= 1, int));
static_assert(TYPE_OF(1ULL >= 1L, int));
static_assert(TYPE_OF(1ULL >= 1L, int));
static_assert(TYPE_OF(1ULL >= 1LL, int));
static_assert(TYPE_OF(1ULL >= 1LL, int));
static_assert(TYPE_OF(1ULL >= 1U, int));
static_assert(TYPE_OF(1ULL >= 1U, int));
static_assert(TYPE_OF(1ULL >= 1UL, int));
static_assert(TYPE_OF(1ULL >= 1UL, int));
static_assert(TYPE_OF(1ULL >= 1ULL, int));
static_assert(TYPE_OF(1ULL >= 1ULL, int));
static_assert(TYPE_OF(1ULL >= 1.0, int));
static_assert(TYPE_OF(1ULL >= 1.0, int));
static_assert(TYPE_OF(1ULL >= 1.0f, int));
static_assert(TYPE_OF(1ULL >= 1.0f, int));
static_assert(TYPE_OF(1ULL >= 1.0L, int));
static_assert(TYPE_OF(1ULL >= 1.0L, int));
static_assert(TYPE_OF(1.0 >= 1, int));
static_assert(TYPE_OF(1.0 >= 1, int));
static_assert(TYPE_OF(1.0 >= 1L, int));
static_assert(TYPE_OF(1.0 >= 1L, int));
static_assert(TYPE_OF(1.0 >= 1LL, int));
static_assert(TYPE_OF(1.0 >= 1LL, int));
static_assert(TYPE_OF(1.0 >= 1U, int));
static_assert(TYPE_OF(1.0 >= 1U, int));
static_assert(TYPE_OF(1.0 >= 1UL, int));
static_assert(TYPE_OF(1.0 >= 1UL, int));
static_assert(TYPE_OF(1.0 >= 1ULL, int));
static_assert(TYPE_OF(1.0 >= 1ULL, int));
static_assert(TYPE_OF(1.0 >= 1.0, int));
static_assert(TYPE_OF(1.0 >= 1.0, int));
static_assert(TYPE_OF(1.0 >= 1.0f, int));
static_assert(TYPE_OF(1.0 >= 1.0f, int));
static_assert(TYPE_OF(1.0 >= 1.0L, int));
static_assert(TYPE_OF(1.0 >= 1.0L, int));
static_assert(TYPE_OF(1.0f >= 1, int));
static_assert(TYPE_OF(1.0f >= 1, int));
static_assert(TYPE_OF(1.0f >= 1L, int));
static_assert(TYPE_OF(1.0f >= 1L, int));
static_assert(TYPE_OF(1.0f >= 1LL, int));
static_assert(TYPE_OF(1.0f >= 1LL, int));
static_assert(TYPE_OF(1.0f >= 1U, int));
static_assert(TYPE_OF(1.0f >= 1U, int));
static_assert(TYPE_OF(1.0f >= 1UL, int));
static_assert(TYPE_OF(1.0f >= 1UL, int));
static_assert(TYPE_OF(1.0f >= 1ULL, int));
static_assert(TYPE_OF(1.0f >= 1ULL, int));
static_assert(TYPE_OF(1.0f >= 1.0, int));
static_assert(TYPE_OF(1.0f >= 1.0, int));
static_assert(TYPE_OF(1.0f >= 1.0f, int));
static_assert(TYPE_OF(1.0f >= 1.0f, int));
static_assert(TYPE_OF(1.0f >= 1.0L, int));
static_assert(TYPE_OF(1.0f >= 1.0L, int));
static_assert(TYPE_OF(1.0L >= 1, int));
static_assert(TYPE_OF(1.0L >= 1, int));
static_assert(TYPE_OF(1.0L >= 1L, int));
static_assert(TYPE_OF(1.0L >= 1L, int));
static_assert(TYPE_OF(1.0L >= 1LL, int));
static_assert(TYPE_OF(1.0L >= 1LL, int));
static_assert(TYPE_OF(1.0L >= 1U, int));
static_assert(TYPE_OF(1.0L >= 1U, int));
static_assert(TYPE_OF(1.0L >= 1UL, int));
static_assert(TYPE_OF(1.0L >= 1UL, int));
static_assert(TYPE_OF(1.0L >= 1ULL, int));
static_assert(TYPE_OF(1.0L >= 1ULL, int));
static_assert(TYPE_OF(1.0L >= 1.0, int));
static_assert(TYPE_OF(1.0L >= 1.0, int));
static_assert(TYPE_OF(1.0L >= 1.0f, int));
static_assert(TYPE_OF(1.0L >= 1.0f, int));
static_assert(TYPE_OF(1.0L >= 1.0L, int));
static_assert(TYPE_OF(1.0L >= 1.0L, int));
static_assert(TYPE_OF(1 < 1, int));
static_assert(TYPE_OF(1 < 1, int));
static_assert(TYPE_OF(1 < 1L, int));
static_assert(TYPE_OF(1 < 1L, int));
static_assert(TYPE_OF(1 < 1LL, int));
static_assert(TYPE_OF(1 < 1LL, int));
static_assert(TYPE_OF(1 < 1U, int));
static_assert(TYPE_OF(1 < 1U, int));
static_assert(TYPE_OF(1 < 1UL, int));
static_assert(TYPE_OF(1 < 1UL, int));
static_assert(TYPE_OF(1 < 1ULL, int));
static_assert(TYPE_OF(1 < 1ULL, int));
static_assert(TYPE_OF(1 < 1.0, int));
static_assert(TYPE_OF(1 < 1.0, int));
static_assert(TYPE_OF(1 < 1.0f, int));
static_assert(TYPE_OF(1 < 1.0f, int));
static_assert(TYPE_OF(1 < 1.0L, int));
static_assert(TYPE_OF(1 < 1.0L, int));
static_assert(TYPE_OF(1L < 1, int));
static_assert(TYPE_OF(1L < 1, int));
static_assert(TYPE_OF(1L < 1L, int));
static_assert(TYPE_OF(1L < 1L, int));
static_assert(TYPE_OF(1L < 1LL, int));
static_assert(TYPE_OF(1L < 1LL, int));
static_assert(TYPE_OF(1L < 1U, int));
static_assert(TYPE_OF(1L < 1U, int));
static_assert(TYPE_OF(1L < 1UL, int));
static_assert(TYPE_OF(1L < 1UL, int));
static_assert(TYPE_OF(1L < 1ULL, int));
static_assert(TYPE_OF(1L < 1ULL, int));
static_assert(TYPE_OF(1L < 1.0, int));
static_assert(TYPE_OF(1L < 1.0, int));
static_assert(TYPE_OF(1L < 1.0f, int));
static_assert(TYPE_OF(1L < 1.0f, int));
static_assert(TYPE_OF(1L < 1.0L, int));
static_assert(TYPE_OF(1L < 1.0L, int));
static_assert(TYPE_OF(1LL < 1, int));
static_assert(TYPE_OF(1LL < 1, int));
static_assert(TYPE_OF(1LL < 1L, int));
static_assert(TYPE_OF(1LL < 1L, int));
static_assert(TYPE_OF(1LL < 1LL, int));
static_assert(TYPE_OF(1LL < 1LL, int));
static_assert(TYPE_OF(1LL < 1U, int));
static_assert(TYPE_OF(1LL < 1U, int));
static_assert(TYPE_OF(1LL < 1UL, int));
static_assert(TYPE_OF(1LL < 1UL, int));
static_assert(TYPE_OF(1LL < 1ULL, int));
static_assert(TYPE_OF(1LL < 1ULL, int));
static_assert(TYPE_OF(1LL < 1.0, int));
static_assert(TYPE_OF(1LL < 1.0, int));
static_assert(TYPE_OF(1LL < 1.0f, int));
static_assert(TYPE_OF(1LL < 1.0f, int));
static_assert(TYPE_OF(1LL < 1.0L, int));
static_assert(TYPE_OF(1LL < 1.0L, int));
static_assert(TYPE_OF(1U < 1, int));
static_assert(TYPE_OF(1U < 1, int));
static_assert(TYPE_OF(1U < 1L, int));
static_assert(TYPE_OF(1U < 1L, int));
static_assert(TYPE_OF(1U < 1LL, int));
static_assert(TYPE_OF(1U < 1LL, int));
static_assert(TYPE_OF(1U < 1U, int));
static_assert(TYPE_OF(1U < 1U, int));
static_assert(TYPE_OF(1U < 1UL, int));
static_assert(TYPE_OF(1U < 1UL, int));
static_assert(TYPE_OF(1U < 1ULL, int));
static_assert(TYPE_OF(1U < 1ULL, int));
static_assert(TYPE_OF(1U < 1.0, int));
static_assert(TYPE_OF(1U < 1.0, int));
static_assert(TYPE_OF(1U < 1.0f, int));
static_assert(TYPE_OF(1U < 1.0f, int));
static_assert(TYPE_OF(1U < 1.0L, int));
static_assert(TYPE_OF(1U < 1.0L, int));
static_assert(TYPE_OF(1UL < 1, int));
static_assert(TYPE_OF(1UL < 1, int));
static_assert(TYPE_OF(1UL < 1L, int));
static_assert(TYPE_OF(1UL < 1L, int));
static_assert(TYPE_OF(1UL < 1LL, int));
static_assert(TYPE_OF(1UL < 1LL, int));
static_assert(TYPE_OF(1UL < 1U, int));
static_assert(TYPE_OF(1UL < 1U, int));
static_assert(TYPE_OF(1UL < 1UL, int));
static_assert(TYPE_OF(1UL < 1UL, int));
static_assert(TYPE_OF(1UL < 1ULL, int));
static_assert(TYPE_OF(1UL < 1ULL, int));
static_assert(TYPE_OF(1UL < 1.0, int));
static_assert(TYPE_OF(1UL < 1.0, int));
static_assert(TYPE_OF(1UL < 1.0f, int));
static_assert(TYPE_OF(1UL < 1.0f, int));
static_assert(TYPE_OF(1UL < 1.0L, int));
static_assert(TYPE_OF(1UL < 1.0L, int));
static_assert(TYPE_OF(1ULL < 1, int));
static_assert(TYPE_OF(1ULL < 1, int));
static_assert(TYPE_OF(1ULL < 1L, int));
static_assert(TYPE_OF(1ULL < 1L, int));
static_assert(TYPE_OF(1ULL < 1LL, int));
static_assert(TYPE_OF(1ULL < 1LL, int));
static_assert(TYPE_OF(1ULL < 1U, int));
static_assert(TYPE_OF(1ULL < 1U, int));
static_assert(TYPE_OF(1ULL < 1UL, int));
static_assert(TYPE_OF(1ULL < 1UL, int));
static_assert(TYPE_OF(1ULL < 1ULL, int));
static_assert(TYPE_OF(1ULL < 1ULL, int));
static_assert(TYPE_OF(1ULL < 1.0, int));
static_assert(TYPE_OF(1ULL < 1.0, int));
static_assert(TYPE_OF(1ULL < 1.0f, int));
static_assert(TYPE_OF(1ULL < 1.0f, int));
static_assert(TYPE_OF(1ULL < 1.0L, int));
static_assert(TYPE_OF(1ULL < 1.0L, int));
static_assert(TYPE_OF(1.0 < 1, int));
static_assert(TYPE_OF(1.0 < 1, int));
static_assert(TYPE_OF(1.0 < 1L, int));
static_assert(TYPE_OF(1.0 < 1L, int));
static_assert(TYPE_OF(1.0 < 1LL, int));
static_assert(TYPE_OF(1.0 < 1LL, int));
static_assert(TYPE_OF(1.0 < 1U, int));
static_assert(TYPE_OF(1.0 < 1U, int));
static_assert(TYPE_OF(1.0 < 1UL, int));
static_assert(TYPE_OF(1.0 < 1UL, int));
static_assert(TYPE_OF(1.0 < 1ULL, int));
static_assert(TYPE_OF(1.0 < 1ULL, int));
static_assert(TYPE_OF(1.0 < 1.0, int));
static_assert(TYPE_OF(1.0 < 1.0, int));
static_assert(TYPE_OF(1.0 < 1.0f, int));
static_assert(TYPE_OF(1.0 < 1.0f, int));
static_assert(TYPE_OF(1.0 < 1.0L, int));
static_assert(TYPE_OF(1.0 < 1.0L, int));
static_assert(TYPE_OF(1.0f < 1, int));
static_assert(TYPE_OF(1.0f < 1, int));
static_assert(TYPE_OF(1.0f < 1L, int));
static_assert(TYPE_OF(1.0f < 1L, int));
static_assert(TYPE_OF(1.0f < 1LL, int));
static_assert(TYPE_OF(1.0f < 1LL, int));
static_assert(TYPE_OF(1.0f < 1U, int));
static_assert(TYPE_OF(1.0f < 1U, int));
static_assert(TYPE_OF(1.0f < 1UL, int));
static_assert(TYPE_OF(1.0f < 1UL, int));
static_assert(TYPE_OF(1.0f < 1ULL, int));
static_assert(TYPE_OF(1.0f < 1ULL, int));
static_assert(TYPE_OF(1.0f < 1.0, int));
static_assert(TYPE_OF(1.0f < 1.0, int));
static_assert(TYPE_OF(1.0f < 1.0f, int));
static_assert(TYPE_OF(1.0f < 1.0f, int));
static_assert(TYPE_OF(1.0f < 1.0L, int));
static_assert(TYPE_OF(1.0f < 1.0L, int));
static_assert(TYPE_OF(1.0L < 1, int));
static_assert(TYPE_OF(1.0L < 1, int));
static_assert(TYPE_OF(1.0L < 1L, int));
static_assert(TYPE_OF(1.0L < 1L, int));
static_assert(TYPE_OF(1.0L < 1LL, int));
static_assert(TYPE_OF(1.0L < 1LL, int));
static_assert(TYPE_OF(1.0L < 1U, int));
static_assert(TYPE_OF(1.0L < 1U, int));
static_assert(TYPE_OF(1.0L < 1UL, int));
static_assert(TYPE_OF(1.0L < 1UL, int));
static_assert(TYPE_OF(1.0L < 1ULL, int));
static_assert(TYPE_OF(1.0L < 1ULL, int));
static_assert(TYPE_OF(1.0L < 1.0, int));
static_assert(TYPE_OF(1.0L < 1.0, int));
static_assert(TYPE_OF(1.0L < 1.0f, int));
static_assert(TYPE_OF(1.0L < 1.0f, int));
static_assert(TYPE_OF(1.0L < 1.0L, int));
static_assert(TYPE_OF(1.0L < 1.0L, int));
static_assert(TYPE_OF(1 <= 1, int));
static_assert(TYPE_OF(1 <= 1, int));
static_assert(TYPE_OF(1 <= 1L, int));
static_assert(TYPE_OF(1 <= 1L, int));
static_assert(TYPE_OF(1 <= 1LL, int));
static_assert(TYPE_OF(1 <= 1LL, int));
static_assert(TYPE_OF(1 <= 1U, int));
static_assert(TYPE_OF(1 <= 1U, int));
static_assert(TYPE_OF(1 <= 1UL, int));
static_assert(TYPE_OF(1 <= 1UL, int));
static_assert(TYPE_OF(1 <= 1ULL, int));
static_assert(TYPE_OF(1 <= 1ULL, int));
static_assert(TYPE_OF(1 <= 1.0, int));
static_assert(TYPE_OF(1 <= 1.0, int));
static_assert(TYPE_OF(1 <= 1.0f, int));
static_assert(TYPE_OF(1 <= 1.0f, int));
static_assert(TYPE_OF(1 <= 1.0L, int));
static_assert(TYPE_OF(1 <= 1.0L, int));
static_assert(TYPE_OF(1L <= 1, int));
static_assert(TYPE_OF(1L <= 1, int));
static_assert(TYPE_OF(1L <= 1L, int));
static_assert(TYPE_OF(1L <= 1L, int));
static_assert(TYPE_OF(1L <= 1LL, int));
static_assert(TYPE_OF(1L <= 1LL, int));
static_assert(TYPE_OF(1L <= 1U, int));
static_assert(TYPE_OF(1L <= 1U, int));
static_assert(TYPE_OF(1L <= 1UL, int));
static_assert(TYPE_OF(1L <= 1UL, int));
static_assert(TYPE_OF(1L <= 1ULL, int));
static_assert(TYPE_OF(1L <= 1ULL, int));
static_assert(TYPE_OF(1L <= 1.0, int));
static_assert(TYPE_OF(1L <= 1.0, int));
static_assert(TYPE_OF(1L <= 1.0f, int));
static_assert(TYPE_OF(1L <= 1.0f, int));
static_assert(TYPE_OF(1L <= 1.0L, int));
static_assert(TYPE_OF(1L <= 1.0L, int));
static_assert(TYPE_OF(1LL <= 1, int));
static_assert(TYPE_OF(1LL <= 1, int));
static_assert(TYPE_OF(1LL <= 1L, int));
static_assert(TYPE_OF(1LL <= 1L, int));
static_assert(TYPE_OF(1LL <= 1LL, int));
static_assert(TYPE_OF(1LL <= 1LL, int));
static_assert(TYPE_OF(1LL <= 1U, int));
static_assert(TYPE_OF(1LL <= 1U, int));
static_assert(TYPE_OF(1LL <= 1UL, int));
static_assert(TYPE_OF(1LL <= 1UL, int));
static_assert(TYPE_OF(1LL <= 1ULL, int));
static_assert(TYPE_OF(1LL <= 1ULL, int));
static_assert(TYPE_OF(1LL <= 1.0, int));
static_assert(TYPE_OF(1LL <= 1.0, int));
static_assert(TYPE_OF(1LL <= 1.0f, int));
static_assert(TYPE_OF(1LL <= 1.0f, int));
static_assert(TYPE_OF(1LL <= 1.0L, int));
static_assert(TYPE_OF(1LL <= 1.0L, int));
static_assert(TYPE_OF(1U <= 1, int));
static_assert(TYPE_OF(1U <= 1, int));
static_assert(TYPE_OF(1U <= 1L, int));
static_assert(TYPE_OF(1U <= 1L, int));
static_assert(TYPE_OF(1U <= 1LL, int));
static_assert(TYPE_OF(1U <= 1LL, int));
static_assert(TYPE_OF(1U <= 1U, int));
static_assert(TYPE_OF(1U <= 1U, int));
static_assert(TYPE_OF(1U <= 1UL, int));
static_assert(TYPE_OF(1U <= 1UL, int));
static_assert(TYPE_OF(1U <= 1ULL, int));
static_assert(TYPE_OF(1U <= 1ULL, int));
static_assert(TYPE_OF(1U <= 1.0, int));
static_assert(TYPE_OF(1U <= 1.0, int));
static_assert(TYPE_OF(1U <= 1.0f, int));
static_assert(TYPE_OF(1U <= 1.0f, int));
static_assert(TYPE_OF(1U <= 1.0L, int));
static_assert(TYPE_OF(1U <= 1.0L, int));
static_assert(TYPE_OF(1UL <= 1, int));
static_assert(TYPE_OF(1UL <= 1, int));
static_assert(TYPE_OF(1UL <= 1L, int));
static_assert(TYPE_OF(1UL <= 1L, int));
static_assert(TYPE_OF(1UL <= 1LL, int));
static_assert(TYPE_OF(1UL <= 1LL, int));
static_assert(TYPE_OF(1UL <= 1U, int));
static_assert(TYPE_OF(1UL <= 1U, int));
static_assert(TYPE_OF(1UL <= 1UL, int));
static_assert(TYPE_OF(1UL <= 1UL, int));
static_assert(TYPE_OF(1UL <= 1ULL, int));
static_assert(TYPE_OF(1UL <= 1ULL, int));
static_assert(TYPE_OF(1UL <= 1.0, int));
static_assert(TYPE_OF(1UL <= 1.0, int));
static_assert(TYPE_OF(1UL <= 1.0f, int));
static_assert(TYPE_OF(1UL <= 1.0f, int));
static_assert(TYPE_OF(1UL <= 1.0L, int));
static_assert(TYPE_OF(1UL <= 1.0L, int));
static_assert(TYPE_OF(1ULL <= 1, int));
static_assert(TYPE_OF(1ULL <= 1, int));
static_assert(TYPE_OF(1ULL <= 1L, int));
static_assert(TYPE_OF(1ULL <= 1L, int));
static_assert(TYPE_OF(1ULL <= 1LL, int));
static_assert(TYPE_OF(1ULL <= 1LL, int));
static_assert(TYPE_OF(1ULL <= 1U, int));
static_assert(TYPE_OF(1ULL <= 1U, int));
static_assert(TYPE_OF(1ULL <= 1UL, int));
static_assert(TYPE_OF(1ULL <= 1UL, int));
static_assert(TYPE_OF(1ULL <= 1ULL, int));
static_assert(TYPE_OF(1ULL <= 1ULL, int));
static_assert(TYPE_OF(1ULL <= 1.0, int));
static_assert(TYPE_OF(1ULL <= 1.0, int));
static_assert(TYPE_OF(1ULL <= 1.0f, int));
static_assert(TYPE_OF(1ULL <= 1.0f, int));
static_assert(TYPE_OF(1ULL <= 1.0L, int));
static_assert(TYPE_OF(1ULL <= 1.0L, int));
static_assert(TYPE_OF(1.0 <= 1, int));
static_assert(TYPE_OF(1.0 <= 1, int));
static_assert(TYPE_OF(1.0 <= 1L, int));
static_assert(TYPE_OF(1.0 <= 1L, int));
static_assert(TYPE_OF(1.0 <= 1LL, int));
static_assert(TYPE_OF(1.0 <= 1LL, int));
static_assert(TYPE_OF(1.0 <= 1U, int));
static_assert(TYPE_OF(1.0 <= 1U, int));
static_assert(TYPE_OF(1.0 <= 1UL, int));
static_assert(TYPE_OF(1.0 <= 1UL, int));
static_assert(TYPE_OF(1.0 <= 1ULL, int));
static_assert(TYPE_OF(1.0 <= 1ULL, int));
static_assert(TYPE_OF(1.0 <= 1.0, int));
static_assert(TYPE_OF(1.0 <= 1.0, int));
static_assert(TYPE_OF(1.0 <= 1.0f, int));
static_assert(TYPE_OF(1.0 <= 1.0f, int));
static_assert(TYPE_OF(1.0 <= 1.0L, int));
static_assert(TYPE_OF(1.0 <= 1.0L, int));
static_assert(TYPE_OF(1.0f <= 1, int));
static_assert(TYPE_OF(1.0f <= 1, int));
static_assert(TYPE_OF(1.0f <= 1L, int));
static_assert(TYPE_OF(1.0f <= 1L, int));
static_assert(TYPE_OF(1.0f <= 1LL, int));
static_assert(TYPE_OF(1.0f <= 1LL, int));
static_assert(TYPE_OF(1.0f <= 1U, int));
static_assert(TYPE_OF(1.0f <= 1U, int));
static_assert(TYPE_OF(1.0f <= 1UL, int));
static_assert(TYPE_OF(1.0f <= 1UL, int));
static_assert(TYPE_OF(1.0f <= 1ULL, int));
static_assert(TYPE_OF(1.0f <= 1ULL, int));
static_assert(TYPE_OF(1.0f <= 1.0, int));
static_assert(TYPE_OF(1.0f <= 1.0, int));
static_assert(TYPE_OF(1.0f <= 1.0f, int));
static_assert(TYPE_OF(1.0f <= 1.0f, int));
static_assert(TYPE_OF(1.0f <= 1.0L, int));
static_assert(TYPE_OF(1.0f <= 1.0L, int));
static_assert(TYPE_OF(1.0L <= 1, int));
static_assert(TYPE_OF(1.0L <= 1, int));
static_assert(TYPE_OF(1.0L <= 1L, int));
static_assert(TYPE_OF(1.0L <= 1L, int));
static_assert(TYPE_OF(1.0L <= 1LL, int));
static_assert(TYPE_OF(1.0L <= 1LL, int));
static_assert(TYPE_OF(1.0L <= 1U, int));
static_assert(TYPE_OF(1.0L <= 1U, int));
static_assert(TYPE_OF(1.0L <= 1UL, int));
static_assert(TYPE_OF(1.0L <= 1UL, int));
static_assert(TYPE_OF(1.0L <= 1ULL, int));
static_assert(TYPE_OF(1.0L <= 1ULL, int));
static_assert(TYPE_OF(1.0L <= 1.0, int));
static_assert(TYPE_OF(1.0L <= 1.0, int));
static_assert(TYPE_OF(1.0L <= 1.0f, int));
static_assert(TYPE_OF(1.0L <= 1.0f, int));
static_assert(TYPE_OF(1.0L <= 1.0L, int));
static_assert(TYPE_OF(1.0L <= 1.0L, int));

// Aritmetic operators

static_assert(TYPE_OF(1 + 1, int));
static_assert(TYPE_OF(1 + 1L, long));
static_assert(TYPE_OF(1 + 1LL, long long));
static_assert(TYPE_OF(1 + 1U, unsigned));
static_assert(TYPE_OF(1 + 1UL, unsigned long));
static_assert(TYPE_OF(1 + 1ULL, unsigned long long));
static_assert(TYPE_OF(1 + 1.0, double));
static_assert(TYPE_OF(1 + 1.0f, float));
static_assert(TYPE_OF(1 + 1.0L, long double));
static_assert(TYPE_OF(1L + 1, long));
static_assert(TYPE_OF(1L + 1L, long));
static_assert(TYPE_OF(1L + 1LL, long long));
static_assert(TYPE_OF(1L + 1U, unsigned long));
static_assert(TYPE_OF(1L + 1UL, unsigned long));
static_assert(TYPE_OF(1L + 1ULL, unsigned long long));
static_assert(TYPE_OF(1L + 1.0, double));
static_assert(TYPE_OF(1L + 1.0f, float));
static_assert(TYPE_OF(1L + 1.0L, long double));
static_assert(TYPE_OF(1LL + 1, long long));
static_assert(TYPE_OF(1LL + 1L, long long));
static_assert(TYPE_OF(1LL + 1LL, long long));
static_assert(TYPE_OF(1LL + 1U, long long));
static_assert(TYPE_OF(1LL + 1UL, long long));
static_assert(TYPE_OF(1LL + 1ULL, unsigned long long));
static_assert(TYPE_OF(1LL + 1.0, double));
static_assert(TYPE_OF(1LL + 1.0f, float));
static_assert(TYPE_OF(1LL + 1.0L, long double));
static_assert(TYPE_OF(1U + 1, unsigned));
static_assert(TYPE_OF(1U + 1L, unsigned long));
static_assert(TYPE_OF(1U + 1LL, long long));
static_assert(TYPE_OF(1U + 1U, unsigned));
static_assert(TYPE_OF(1U + 1UL, unsigned long));
static_assert(TYPE_OF(1U + 1ULL, unsigned long long));
static_assert(TYPE_OF(1U + 1.0, double));
static_assert(TYPE_OF(1U + 1.0f, float));
static_assert(TYPE_OF(1U + 1.0L, long double));
static_assert(TYPE_OF(1UL + 1, unsigned long));
static_assert(TYPE_OF(1UL + 1L, unsigned long));
static_assert(TYPE_OF(1UL + 1LL, long long));
static_assert(TYPE_OF(1UL + 1U, unsigned long));
static_assert(TYPE_OF(1UL + 1UL, unsigned long));
static_assert(TYPE_OF(1UL + 1ULL, unsigned long long));
static_assert(TYPE_OF(1UL + 1.0, double));
static_assert(TYPE_OF(1UL + 1.0f, float));
static_assert(TYPE_OF(1UL + 1.0L, long double));
static_assert(TYPE_OF(1ULL + 1, unsigned long long));
static_assert(TYPE_OF(1ULL + 1L, unsigned long long));
static_assert(TYPE_OF(1ULL + 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL + 1U, unsigned long long));
static_assert(TYPE_OF(1ULL + 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL + 1ULL, unsigned long long));
static_assert(TYPE_OF(1ULL + 1.0, double));
static_assert(TYPE_OF(1ULL + 1.0f, float));
static_assert(TYPE_OF(1ULL + 1.0L, long double));
static_assert(TYPE_OF(1.0 + 1, double));
static_assert(TYPE_OF(1.0 + 1L, double));
static_assert(TYPE_OF(1.0 + 1LL, double));
static_assert(TYPE_OF(1.0 + 1U, double));
static_assert(TYPE_OF(1.0 + 1UL, double));
static_assert(TYPE_OF(1.0 + 1ULL, double));
static_assert(TYPE_OF(1.0 + 1.0, double));
static_assert(TYPE_OF(1.0 + 1.0f, double));
static_assert(TYPE_OF(1.0 + 1.0L, long double));
static_assert(TYPE_OF(1.0f + 1, float));
static_assert(TYPE_OF(1.0f + 1L, float));
static_assert(TYPE_OF(1.0f + 1LL, float));
static_assert(TYPE_OF(1.0f + 1U, float));
static_assert(TYPE_OF(1.0f + 1UL, float));
static_assert(TYPE_OF(1.0f + 1ULL, float));
static_assert(TYPE_OF(1.0f + 1.0, double));
static_assert(TYPE_OF(1.0f + 1.0f, float));
static_assert(TYPE_OF(1.0f + 1.0L, long double));
static_assert(TYPE_OF(1.0L + 1, long double));
static_assert(TYPE_OF(1.0L + 1L, long double));
static_assert(TYPE_OF(1.0L + 1LL, long double));
static_assert(TYPE_OF(1.0L + 1U, long double));
static_assert(TYPE_OF(1.0L + 1UL, long double));
static_assert(TYPE_OF(1.0L + 1ULL, long double));
static_assert(TYPE_OF(1.0L + 1.0, long double));
static_assert(TYPE_OF(1.0L + 1.0f, long double));
static_assert(TYPE_OF(1.0L + 1.0L, long double));



static_assert(TYPE_OF(1 - 1, int));
static_assert(TYPE_OF(1 - 1L, long));
static_assert(TYPE_OF(1 - 1LL, long long));
static_assert(TYPE_OF(1 - 1U, unsigned));
static_assert(TYPE_OF(1 - 1UL, unsigned long));
static_assert(TYPE_OF(1 - 1ULL, unsigned long long));
static_assert(TYPE_OF(1 - 1.0, double));
static_assert(TYPE_OF(1 - 1.0f, float));
static_assert(TYPE_OF(1 - 1.0L, long double));
static_assert(TYPE_OF(1L - 1, long));
static_assert(TYPE_OF(1L - 1L, long));
static_assert(TYPE_OF(1L - 1LL, long long));
static_assert(TYPE_OF(1L - 1U, unsigned long));
static_assert(TYPE_OF(1L - 1UL, unsigned long));
static_assert(TYPE_OF(1L - 1ULL, unsigned long long));
static_assert(TYPE_OF(1L - 1.0, double));
static_assert(TYPE_OF(1L - 1.0f, float));
static_assert(TYPE_OF(1L - 1.0L, long double));
static_assert(TYPE_OF(1LL - 1, long long));
static_assert(TYPE_OF(1LL - 1L, long long));
static_assert(TYPE_OF(1LL - 1LL, long long));
static_assert(TYPE_OF(1LL - 1U, long long));
static_assert(TYPE_OF(1LL - 1UL, long long));
static_assert(TYPE_OF(1LL - 1ULL, unsigned long long));
static_assert(TYPE_OF(1LL - 1.0, double));
static_assert(TYPE_OF(1LL - 1.0f, float));
static_assert(TYPE_OF(1LL - 1.0L, long double));
static_assert(TYPE_OF(1U - 1, unsigned));
static_assert(TYPE_OF(1U - 1L, unsigned long));
static_assert(TYPE_OF(1U - 1LL, long long));
static_assert(TYPE_OF(1U - 1U, unsigned));
static_assert(TYPE_OF(1U - 1UL, unsigned long));
static_assert(TYPE_OF(1U - 1ULL, unsigned long long));
static_assert(TYPE_OF(1U - 1.0, double));
static_assert(TYPE_OF(1U - 1.0f, float));
static_assert(TYPE_OF(1U - 1.0L, long double));
static_assert(TYPE_OF(1UL - 1, unsigned long));
static_assert(TYPE_OF(1UL - 1L, unsigned long));
static_assert(TYPE_OF(1UL - 1LL, long long));
static_assert(TYPE_OF(1UL - 1U, unsigned long));
static_assert(TYPE_OF(1UL - 1UL, unsigned long));
static_assert(TYPE_OF(1UL - 1ULL, unsigned long long));
static_assert(TYPE_OF(1UL - 1.0, double));
static_assert(TYPE_OF(1UL - 1.0f, float));
static_assert(TYPE_OF(1UL - 1.0L, long double));
static_assert(TYPE_OF(1ULL - 1, unsigned long long));
static_assert(TYPE_OF(1ULL - 1L, unsigned long long));
static_assert(TYPE_OF(1ULL - 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL - 1U, unsigned long long));
static_assert(TYPE_OF(1ULL - 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL - 1ULL, unsigned long long));
static_assert(TYPE_OF(1ULL - 1.0, double));
static_assert(TYPE_OF(1ULL - 1.0f, float));
static_assert(TYPE_OF(1ULL - 1.0L, long double));
static_assert(TYPE_OF(1.0 - 1, double));
static_assert(TYPE_OF(1.0 - 1L, double));
static_assert(TYPE_OF(1.0 - 1LL, double));
static_assert(TYPE_OF(1.0 - 1U, double));
static_assert(TYPE_OF(1.0 - 1UL, double));
static_assert(TYPE_OF(1.0 - 1ULL, double));
static_assert(TYPE_OF(1.0 - 1.0, double));
static_assert(TYPE_OF(1.0 - 1.0f, double));
static_assert(TYPE_OF(1.0 - 1.0L, long double));
static_assert(TYPE_OF(1.0f - 1, float));
static_assert(TYPE_OF(1.0f - 1L, float));
static_assert(TYPE_OF(1.0f - 1LL, float));
static_assert(TYPE_OF(1.0f - 1U, float));
static_assert(TYPE_OF(1.0f - 1UL, float));
static_assert(TYPE_OF(1.0f - 1ULL, float));
static_assert(TYPE_OF(1.0f - 1.0, double));
static_assert(TYPE_OF(1.0f - 1.0f, float));
static_assert(TYPE_OF(1.0f - 1.0L, long double));
static_assert(TYPE_OF(1.0L - 1, long double));
static_assert(TYPE_OF(1.0L - 1L, long double));
static_assert(TYPE_OF(1.0L - 1LL, long double));
static_assert(TYPE_OF(1.0L - 1U, long double));
static_assert(TYPE_OF(1.0L - 1UL, long double));
static_assert(TYPE_OF(1.0L - 1ULL, long double));
static_assert(TYPE_OF(1.0L - 1.0, long double));
static_assert(TYPE_OF(1.0L - 1.0f, long double));
static_assert(TYPE_OF(1.0L - 1.0L, long double));



static_assert(TYPE_OF(1 * 1, int));
static_assert(TYPE_OF(1 * 1L, long));
static_assert(TYPE_OF(1 * 1LL, long long));
static_assert(TYPE_OF(1 * 1U, unsigned));
static_assert(TYPE_OF(1 * 1UL, unsigned long));
static_assert(TYPE_OF(1 * 1ULL, unsigned long long));
static_assert(TYPE_OF(1 * 1.0, double));
static_assert(TYPE_OF(1 * 1.0f, float));
static_assert(TYPE_OF(1 * 1.0L, long double));
static_assert(TYPE_OF(1L * 1, long));
static_assert(TYPE_OF(1L * 1L, long));
static_assert(TYPE_OF(1L * 1LL, long long));
static_assert(TYPE_OF(1L * 1U, unsigned long));
static_assert(TYPE_OF(1L * 1UL, unsigned long));
static_assert(TYPE_OF(1L * 1ULL, unsigned long long));
static_assert(TYPE_OF(1L * 1.0, double));
static_assert(TYPE_OF(1L * 1.0f, float));
static_assert(TYPE_OF(1L * 1.0L, long double));
static_assert(TYPE_OF(1LL * 1, long long));
static_assert(TYPE_OF(1LL * 1L, long long));
static_assert(TYPE_OF(1LL * 1LL, long long));
static_assert(TYPE_OF(1LL * 1U, long long));
static_assert(TYPE_OF(1LL * 1UL, long long));
static_assert(TYPE_OF(1LL * 1ULL, unsigned long long));
static_assert(TYPE_OF(1LL * 1.0, double));
static_assert(TYPE_OF(1LL * 1.0f, float));
static_assert(TYPE_OF(1LL * 1.0L, long double));
static_assert(TYPE_OF(1U * 1, unsigned));
static_assert(TYPE_OF(1U * 1L, unsigned long));
static_assert(TYPE_OF(1U * 1LL, long long));
static_assert(TYPE_OF(1U * 1U, unsigned));
static_assert(TYPE_OF(1U * 1UL, unsigned long));
static_assert(TYPE_OF(1U * 1ULL, unsigned long long));
static_assert(TYPE_OF(1U * 1.0, double));
static_assert(TYPE_OF(1U * 1.0f, float));
static_assert(TYPE_OF(1U * 1.0L, long double));
static_assert(TYPE_OF(1UL * 1, unsigned long));
static_assert(TYPE_OF(1UL * 1L, unsigned long));
static_assert(TYPE_OF(1UL * 1LL, long long));
static_assert(TYPE_OF(1UL * 1U, unsigned long));
static_assert(TYPE_OF(1UL * 1UL, unsigned long));
static_assert(TYPE_OF(1UL * 1ULL, unsigned long long));
static_assert(TYPE_OF(1UL * 1.0, double));
static_assert(TYPE_OF(1UL * 1.0f, float));
static_assert(TYPE_OF(1UL * 1.0L, long double));
static_assert(TYPE_OF(1ULL * 1, unsigned long long));
static_assert(TYPE_OF(1ULL * 1L, unsigned long long));
static_assert(TYPE_OF(1ULL * 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL * 1U, unsigned long long));
static_assert(TYPE_OF(1ULL * 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL * 1ULL, unsigned long long));
static_assert(TYPE_OF(1ULL * 1.0, double));
static_assert(TYPE_OF(1ULL * 1.0f, float));
static_assert(TYPE_OF(1ULL * 1.0L, long double));
static_assert(TYPE_OF(1.0 * 1, double));
static_assert(TYPE_OF(1.0 * 1L, double));
static_assert(TYPE_OF(1.0 * 1LL, double));
static_assert(TYPE_OF(1.0 * 1U, double));
static_assert(TYPE_OF(1.0 * 1UL, double));
static_assert(TYPE_OF(1.0 * 1ULL, double));
static_assert(TYPE_OF(1.0 * 1.0, double));
static_assert(TYPE_OF(1.0 * 1.0f, double));
static_assert(TYPE_OF(1.0 * 1.0L, long double));
static_assert(TYPE_OF(1.0f * 1, float));
static_assert(TYPE_OF(1.0f * 1L, float));
static_assert(TYPE_OF(1.0f * 1LL, float));
static_assert(TYPE_OF(1.0f * 1U, float));
static_assert(TYPE_OF(1.0f * 1UL, float));
static_assert(TYPE_OF(1.0f * 1ULL, float));
static_assert(TYPE_OF(1.0f * 1.0, double));
static_assert(TYPE_OF(1.0f * 1.0f, float));
static_assert(TYPE_OF(1.0f * 1.0L, long double));
static_assert(TYPE_OF(1.0L * 1, long double));
static_assert(TYPE_OF(1.0L * 1L, long double));
static_assert(TYPE_OF(1.0L * 1LL, long double));
static_assert(TYPE_OF(1.0L * 1U, long double));
static_assert(TYPE_OF(1.0L * 1UL, long double));
static_assert(TYPE_OF(1.0L * 1ULL, long double));
static_assert(TYPE_OF(1.0L * 1.0, long double));
static_assert(TYPE_OF(1.0L * 1.0f, long double));
static_assert(TYPE_OF(1.0L * 1.0L, long double));



static_assert(TYPE_OF(1 / 1, int));
static_assert(TYPE_OF(1 / 1L, long));
static_assert(TYPE_OF(1 / 1LL, long long));
static_assert(TYPE_OF(1 / 1U, unsigned));
static_assert(TYPE_OF(1 / 1UL, unsigned long));
static_assert(TYPE_OF(1 / 1ULL, unsigned long long));
static_assert(TYPE_OF(1 / 1.0, double));
static_assert(TYPE_OF(1 / 1.0f, float));
static_assert(TYPE_OF(1 / 1.0L, long double));
static_assert(TYPE_OF(1L / 1, long));
static_assert(TYPE_OF(1L / 1L, long));
static_assert(TYPE_OF(1L / 1LL, long long));
static_assert(TYPE_OF(1L / 1U, unsigned long));
static_assert(TYPE_OF(1L / 1UL, unsigned long));
static_assert(TYPE_OF(1L / 1ULL, unsigned long long));
static_assert(TYPE_OF(1L / 1.0, double));
static_assert(TYPE_OF(1L / 1.0f, float));
static_assert(TYPE_OF(1L / 1.0L, long double));
static_assert(TYPE_OF(1LL / 1, long long));
static_assert(TYPE_OF(1LL / 1L, long long));
static_assert(TYPE_OF(1LL / 1LL, long long));
static_assert(TYPE_OF(1LL / 1U, long long));
static_assert(TYPE_OF(1LL / 1UL, long long));
static_assert(TYPE_OF(1LL / 1ULL, unsigned long long));
static_assert(TYPE_OF(1LL / 1.0, double));
static_assert(TYPE_OF(1LL / 1.0f, float));
static_assert(TYPE_OF(1LL / 1.0L, long double));
static_assert(TYPE_OF(1U / 1, unsigned));
static_assert(TYPE_OF(1U / 1L, unsigned long));
static_assert(TYPE_OF(1U / 1LL, long long));
static_assert(TYPE_OF(1U / 1U, unsigned));
static_assert(TYPE_OF(1U / 1UL, unsigned long));
static_assert(TYPE_OF(1U / 1ULL, unsigned long long));
static_assert(TYPE_OF(1U / 1.0, double));
static_assert(TYPE_OF(1U / 1.0f, float));
static_assert(TYPE_OF(1U / 1.0L, long double));
static_assert(TYPE_OF(1UL / 1, unsigned long));
static_assert(TYPE_OF(1UL / 1L, unsigned long));
static_assert(TYPE_OF(1UL / 1LL, long long));
static_assert(TYPE_OF(1UL / 1U, unsigned long));
static_assert(TYPE_OF(1UL / 1UL, unsigned long));
static_assert(TYPE_OF(1UL / 1ULL, unsigned long long));
static_assert(TYPE_OF(1UL / 1.0, double));
static_assert(TYPE_OF(1UL / 1.0f, float));
static_assert(TYPE_OF(1UL / 1.0L, long double));
static_assert(TYPE_OF(1ULL / 1, unsigned long long));
static_assert(TYPE_OF(1ULL / 1L, unsigned long long));
static_assert(TYPE_OF(1ULL / 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL / 1U, unsigned long long));
static_assert(TYPE_OF(1ULL / 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL / 1ULL, unsigned long long));
static_assert(TYPE_OF(1ULL / 1.0, double));
static_assert(TYPE_OF(1ULL / 1.0f, float));
static_assert(TYPE_OF(1ULL / 1.0L, long double));
static_assert(TYPE_OF(1.0 / 1, double));
static_assert(TYPE_OF(1.0 / 1L, double));
static_assert(TYPE_OF(1.0 / 1LL, double));
static_assert(TYPE_OF(1.0 / 1U, double));
static_assert(TYPE_OF(1.0 / 1UL, double));
static_assert(TYPE_OF(1.0 / 1ULL, double));
static_assert(TYPE_OF(1.0 / 1.0, double));
static_assert(TYPE_OF(1.0 / 1.0f, double));
static_assert(TYPE_OF(1.0 / 1.0L, long double));
static_assert(TYPE_OF(1.0f / 1, float));
static_assert(TYPE_OF(1.0f / 1L, float));
static_assert(TYPE_OF(1.0f / 1LL, float));
static_assert(TYPE_OF(1.0f / 1U, float));
static_assert(TYPE_OF(1.0f / 1UL, float));
static_assert(TYPE_OF(1.0f / 1ULL, float));
static_assert(TYPE_OF(1.0f / 1.0, double));
static_assert(TYPE_OF(1.0f / 1.0f, float));
static_assert(TYPE_OF(1.0f / 1.0L, long double));
static_assert(TYPE_OF(1.0L / 1, long double));
static_assert(TYPE_OF(1.0L / 1L, long double));
static_assert(TYPE_OF(1.0L / 1LL, long double));
static_assert(TYPE_OF(1.0L / 1U, long double));
static_assert(TYPE_OF(1.0L / 1UL, long double));
static_assert(TYPE_OF(1.0L / 1ULL, long double));
static_assert(TYPE_OF(1.0L / 1.0, long double));
static_assert(TYPE_OF(1.0L / 1.0f, long double));
static_assert(TYPE_OF(1.0L / 1.0L, long double));



static_assert(TYPE_OF(1 % 1, int));
static_assert(TYPE_OF(1 % 1L, long));
static_assert(TYPE_OF(1 % 1LL, long long));
static_assert(TYPE_OF(1 % 1U, unsigned));
static_assert(TYPE_OF(1 % 1UL, unsigned long));
static_assert(TYPE_OF(1 % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1 % 1.0, double));
//static_assert(TYPE_OF(1 % 1.0f, float));
//static_assert(TYPE_OF(1 % 1.0L, long double));
static_assert(TYPE_OF(1L % 1, long));
static_assert(TYPE_OF(1L % 1L, long));
static_assert(TYPE_OF(1L % 1LL, long long));
static_assert(TYPE_OF(1L % 1U, unsigned long));
static_assert(TYPE_OF(1L % 1UL, unsigned long));
static_assert(TYPE_OF(1L % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1L % 1.0, double));
//static_assert(TYPE_OF(1L % 1.0f, float));
//static_assert(TYPE_OF(1L % 1.0L, long double));
static_assert(TYPE_OF(1LL % 1, long long));
static_assert(TYPE_OF(1LL % 1L, long long));
static_assert(TYPE_OF(1LL % 1LL, long long));
static_assert(TYPE_OF(1LL % 1U, long long));
static_assert(TYPE_OF(1LL % 1UL, long long));
static_assert(TYPE_OF(1LL % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1LL % 1.0, double));
//static_assert(TYPE_OF(1LL % 1.0f, float));
//static_assert(TYPE_OF(1LL % 1.0L, long double));
static_assert(TYPE_OF(1U % 1, unsigned));
static_assert(TYPE_OF(1U % 1L, unsigned long));
static_assert(TYPE_OF(1U % 1LL, long long));
static_assert(TYPE_OF(1U % 1U, unsigned));
static_assert(TYPE_OF(1U % 1UL, unsigned long));
static_assert(TYPE_OF(1U % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1U % 1.0, double));
//static_assert(TYPE_OF(1U % 1.0f, float));
//static_assert(TYPE_OF(1U % 1.0L, long double));
static_assert(TYPE_OF(1UL % 1, unsigned long));
static_assert(TYPE_OF(1UL % 1L, unsigned long));
static_assert(TYPE_OF(1UL % 1LL, long long));
static_assert(TYPE_OF(1UL % 1U, unsigned long));
static_assert(TYPE_OF(1UL % 1UL, unsigned long));
static_assert(TYPE_OF(1UL % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1UL % 1.0, double));
//static_assert(TYPE_OF(1UL % 1.0f, float));
//static_assert(TYPE_OF(1UL % 1.0L, long double));
static_assert(TYPE_OF(1ULL % 1, unsigned long long));
static_assert(TYPE_OF(1ULL % 1L, unsigned long long));
static_assert(TYPE_OF(1ULL % 1LL, unsigned long long));
static_assert(TYPE_OF(1ULL % 1U, unsigned long long));
static_assert(TYPE_OF(1ULL % 1UL, unsigned long long));
static_assert(TYPE_OF(1ULL % 1ULL, unsigned long long));
//static_assert(TYPE_OF(1ULL % 1.0, double));
//static_assert(TYPE_OF(1ULL % 1.0f, float));
//static_assert(TYPE_OF(1ULL % 1.0L, long double));
//static_assert(TYPE_OF(1.0 % 1, double));
//static_assert(TYPE_OF(1.0 % 1L, double));
//static_assert(TYPE_OF(1.0 % 1LL, double));
//static_assert(TYPE_OF(1.0 % 1U, double));
//static_assert(TYPE_OF(1.0 % 1UL, double));
//static_assert(TYPE_OF(1.0 % 1ULL, double));
//static_assert(TYPE_OF(1.0 % 1.0, double));
//static_assert(TYPE_OF(1.0 % 1.0f, double));
//static_assert(TYPE_OF(1.0 % 1.0L, long double));
//static_assert(TYPE_OF(1.0f % 1, float));
//static_assert(TYPE_OF(1.0f % 1L, float));
//static_assert(TYPE_OF(1.0f % 1LL, float));
//static_assert(TYPE_OF(1.0f % 1U, float));
//static_assert(TYPE_OF(1.0f % 1UL, float));
//static_assert(TYPE_OF(1.0f % 1ULL, float));
//static_assert(TYPE_OF(1.0f % 1.0, double));
//static_assert(TYPE_OF(1.0f % 1.0f, float));
//static_assert(TYPE_OF(1.0f % 1.0L, long double));
//static_assert(TYPE_OF(1.0L % 1, long double));
//static_assert(TYPE_OF(1.0L % 1L, long double));
//static_assert(TYPE_OF(1.0L % 1LL, long double));
//static_assert(TYPE_OF(1.0L % 1U, long double));
//static_assert(TYPE_OF(1.0L % 1UL, long double));
//static_assert(TYPE_OF(1.0L % 1ULL, long double));
//static_assert(TYPE_OF(1.0L % 1.0, long double));
//static_assert(TYPE_OF(1.0L % 1.0f, long double));
//static_assert(TYPE_OF(1.0L % 1.0L, long double));

#endif
