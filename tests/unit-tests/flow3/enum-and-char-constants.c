#pragma safety enable

/*
   Compile-time integer constants are seeded into flow3 from the value the
   parser folded, so they work in flow-checked comparisons (compile_assert):

     - enumerators           (RED, GREEN, ...)   -- signed; may be negative
     - character constants   ('A', '\n')         -- via the numeric-literal path

   Use compile_assert (flow-checked), not static_assert (C11 compile-time).
*/

enum Color { RED, GREEN = 5, BLUE };      /* 0, 5, 6 */
enum Signed { NEG = -3, ZERO = 0, POS = 3 };

void enumerators(void)
{
    compile_assert(RED == 0);
    compile_assert(GREEN == 5);
    compile_assert(BLUE == 6);            /* GREEN + 1 */
}

void negative_enumerators(void)
{
    compile_assert(NEG == -3);
    compile_assert(POS - NEG == 6);
}

void character_constants(void)
{
    compile_assert('A' == 65);
    compile_assert('\n' == 10);
    compile_assert('0' == 48);
}

/* Enumerators combine with other constants and fold like any integer value. */
void combined(void)
{
    compile_assert(GREEN + 'A' == 70);
    compile_assert(BLUE * 2 == 12);
}
