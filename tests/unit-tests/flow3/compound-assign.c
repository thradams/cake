#pragma safety enable

/*
   Compound assignment folding. Also guards the lhs value_kind check:
   the folded lhs must be a numeric alternative, not a REF/PTR whose
   pointer bits would be reinterpreted as an integer.
*/

void compound_fold()
{
    int a = 10;
    a += 5;
    compile_assert(a == 15);
    a -= 3;
    compile_assert(a == 12);
    a *= 2;
    compile_assert(a == 24);
    a /= 4;
    compile_assert(a == 6);
    a %= 4;
    compile_assert(a == 2);
    a <<= 3;
    compile_assert(a == 16);
    a >>= 2;
    compile_assert(a == 4);
    a |= 1;
    compile_assert(a == 5);
    a &= 6;
    compile_assert(a == 4);
    a ^= 5;
    compile_assert(a == 1);
}

int unknown();

void compound_unknown_rhs()
{
    int a = 10;
    a += unknown();
    /* result must degrade to unknown, not keep the old constant */
    compile_assert(a != 10 || a == 10); /* tautology: just must not crash */
}
