#pragma flow enable

void test_postfix_increment_basic(void)
{
    int i = 0;
    i++;
    compile_assert(i == 1);
}

void test_postfix_increment_chain(void)
{
    int i = 0;
    i++;
    i++;
    compile_assert(i == 2);
}

void test_postfix_increment_from_nonzero(void)
{
    int i = 5;
    i++;
    compile_assert(i == 6);
}

void test_postfix_decrement_basic(void)
{
    int i = 3;
    i--;
    compile_assert(i == 2);
}

void test_postfix_decrement_to_zero(void)
{
    int i = 1;
    i--;
    compile_assert(i == 0);
}

void test_postfix_decrement_below_zero(void)
{
    int i = 0;
    i--;
    compile_assert(i == -1);
}

void test_postfix_increment_init_expression_value(void)
{
    int i = 0;
    int j = i++;    /* j gets OLD value (0); i becomes 1 */
    compile_assert(j == 0);
    compile_assert(i == 1);
}

void test_postfix_decrement_init_expression_value(void)
{
    int i = 7;
    int j = i--;    /* j gets OLD value (7); i becomes 6 */
    compile_assert(j == 7);
    compile_assert(i == 6);
}

void test_postfix_increment_assign_expression_value(void)
{
    int i = 0;
    int j;
    j = i++;        /* j gets OLD value (0); i becomes 1 */
    compile_assert(j == 0);
    compile_assert(i == 1);
}

void test_postfix_decrement_assign_expression_value(void)
{
    int i = 4;
    int j;
    j = i--;        /* j gets OLD value (4); i becomes 3 */
    compile_assert(j == 4);
    compile_assert(i == 3);
}

void test_prefix_increment_basic(void)
{
    int i = 0;
    ++i;
    compile_assert(i == 1);
}

void test_prefix_increment_chain(void)
{
    int i = 0;
    ++i;
    ++i;
    ++i;
    compile_assert(i == 3);
}

void test_prefix_decrement_basic(void)
{
    int i = 4;
    --i;
    compile_assert(i == 3);
}

void test_prefix_increment_init_expression_value(void)
{
    int i = 0;
    int j = ++i;    /* j and i both become 1 */
    compile_assert(j == 1);
    compile_assert(i == 1);
}

void test_prefix_decrement_init_expression_value(void)
{
    int i = 5;
    int j = --i;    /* j and i both become 4 */
    compile_assert(j == 4);
    compile_assert(i == 4);
}

void test_prefix_increment_assign_expression_value(void)
{
    int i = 0;
    int j;
    j = ++i;        /* j and i both become 1 */
    compile_assert(j == 1);
    compile_assert(i == 1);
}

void test_prefix_decrement_assign_expression_value(void)
{
    int i = 5;
    int j;
    j = --i;        /* j and i both become 4 */
    compile_assert(j == 4);
    compile_assert(i == 4);
}

void test_comma_postfix_then_assert(void)
{
    int i = 0;
    i++, compile_assert(i == 1);
}

void test_comma_prefix_then_assert(void)
{
    int i = 0;
    ++i, compile_assert(i == 1);
}

void test_mixed_increment_decrement(void)
{
    int i = 0;
    i++;    /* 1 */
    ++i;    /* 2 */
    i--;    /* 1 */
    --i;    /* 0 */
    compile_assert(i == 0);
}


void test_add_tracked_plus_literal(void)
{
    int i = 0;
    i++;                /* i == 1 */
    int j = i + 2;
    compile_assert(j == 3);
}

void test_add_tracked_plus_tracked(void)
{
    int a = 0;
    int b = 0;
    a++;                /* a == 1 */
    b++; b++;           /* b == 2 */
    int c = a + b;
    compile_assert(c == 3);
}

void test_sub_tracked_minus_literal(void)
{
    int i = 0;
    i++; i++; i++;      /* i == 3 */
    int j = i - 1;
    compile_assert(j == 2);
}

void test_add_zero_identity_tracked(void)   /* x + 0 == x */
{
    int x = 0;
    x++;                /* x == 1 */
    int y = x + 0;
    compile_assert(y == 1);
}

void test_zero_plus_tracked(void)           /* 0 + x == x */
{
    int x = 0;
    x++; x++;           /* x == 2 */
    int y = 0 + x;
    compile_assert(y == 2);
}

void test_sub_zero_identity_tracked(void)   /* x - 0 == x */
{
    int x = 0;
    x++;                /* x == 1 */
    int y = x - 0;
    compile_assert(y == 1);
}

void test_sub_self_is_zero(void)            /* x - x == 0 */
{
    int x = 0;
    x++; x++;           /* x == 2, value doesn't matter for this identity */
    int y = x - x;
    compile_assert(y == 0);
}

void test_mul_one_identity_right(void)      /* x * 1 == x */
{
    int x = 0;
    x++; x++;           /* x == 2 */
    int y = x * 1;
    compile_assert(y == 2);
}

void test_mul_one_identity_left(void)       /* 1 * x == x */
{
    int x = 0;
    x++; x++; x++;      /* x == 3 */
    int y = 1 * x;
    compile_assert(y == 3);
}

void test_mul_zero_absorb_right(void)       /* x * 0 == 0 */
{
    int x = 0;
    x++;                /* x == 1 — value doesn't matter */
    int y = x * 0;
    compile_assert(y == 0);
}

void test_mul_zero_absorb_left(void)        /* 0 * x == 0 */
{
    int x = 0;
    x++; x++;           /* x == 2 — value doesn't matter */
    int y = 0 * x;
    compile_assert(y == 0);
}

void test_mul_tracked_times_tracked(void)
{
    int a = 0;
    int b = 0;
    a++; a++;           /* a == 2 */
    b++; b++; b++;      /* b == 3 */
    int c = a * b;
    compile_assert(c == 6);
}


#if 0

/* Expected: "compile_assert failed" — i is 1 after i++, not 0 */
void test_FAIL_postfix_wrong_value(void)
{
    int i = 0;
    i++;
    compile_assert(i == 0); /*FAIL*/
}

/* Expected: "compile_assert failed" — j=i++ gives OLD value (0), not 1 */
void test_FAIL_postfix_init_expression_value_wrong(void)
{
    int i = 0;
    int j = i++;
    compile_assert(j == 1); /*FAIL*/
}

/* Expected: "compile_assert failed" — j=++i gives NEW value (1), not 0 */
void test_FAIL_prefix_init_expression_value_wrong(void)
{
    int i = 0;
    int j = ++i;
    compile_assert(j == 0); /*FAIL*/
}

/* Expected: "compile_assert failed" — 2 * 3 == 6, not 5 */
void test_FAIL_mul_wrong_result(void)
{
    int a = 0; int b = 0;
    a++; a++;       /* a == 2 */
    b++; b++; b++;  /* b == 3 */
    int c = a * b;
    compile_assert(c == 5); /*FAIL*/
}

/* Expected: "could not be proven" — x is unknown after unknown_fn() */
void test_FAIL_unknown_operand(void)
{
    int x = unknown_fn();
    x++;
    compile_assert(x == 1); /*FAIL: x was unknown, still unknown after ++*/
}

#endif
