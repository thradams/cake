/*
   An initializer is converted to the type of the object it initializes.
   bool is lowered to unsigned char, so the conversion must be emitted:
   256 and 0.5 are true, not the low byte of the value. The same for the
   members of arrays, structs and compound literals, with static and
   automatic storage.
*/

#include <stdio.h>

#pragma CAKE diagnostic ignored 74 /* constants that are not 0 or 1 converted to bool are what is tested */

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct flags
{
    bool on;
    unsigned char byte;
    unsigned _BitInt(3) small;
};

static bool file_scalar = 256;
static bool file_array[2] = { 2, 0 };
static struct flags file_struct = { 256, 1, 9 };

static int id(int x)
{
    return x;
}

static void static_storage(void)
{
    static bool local_static = 512;
    static bool local_static_array[2] = { 0, 256 };

    CHECK(file_scalar == 1);
    CHECK(file_array[0] == 1 && file_array[1] == 0);
    CHECK(file_struct.on == 1 && file_struct.byte == 1 && file_struct.small == 1);
    CHECK(local_static == 1);
    CHECK(local_static_array[0] == 0 && local_static_array[1] == 1);
}

static void constants(void)
{
    bool scalar = 512;
    bool array[3] = { 2, 256, 0 };
    struct flags s = { 256, 1, 9 };
    struct flags from_double = { .on = 0.5 };

    CHECK(scalar == 1);
    CHECK(array[0] == 1 && array[1] == 1 && array[2] == 0);
    CHECK(s.on == 1 && s.byte == 1 && s.small == 1);
    CHECK(from_double.on == 1);
}

static void not_constants(void)
{
    bool scalar = id(512);
    bool array[3] = { id(2), id(256), id(0) };
    struct flags s = { id(256), id(257), id(9) };
    struct flags from_double = { .on = id(1) / 4.0 };

    CHECK(scalar == 1);
    CHECK(array[0] == 1 && array[1] == 1 && array[2] == 0);
    CHECK(s.on == 1 && s.byte == 1 && s.small == 1);
    CHECK(from_double.on == 1);
}

static void compound_literals(void)
{
    bool* p = &(bool){ id(256) };
    struct flags* q = &(struct flags){ id(256), 2 };
    bool* r = (bool[2]){ id(0), id(1024) };

    CHECK(*p == 1);
    CHECK(q->on == 1 && q->byte == 2 && q->small == 0);
    CHECK(r[0] == 0 && r[1] == 1);
}

int main(void)
{
    static_storage();
    constants();
    not_constants();
    compound_literals();

    return failures;
}
