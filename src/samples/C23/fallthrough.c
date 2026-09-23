/*
   Implicit fall-through (warning 88) and the [[fallthrough]] attribute are
   checked, but only at the switch body's own statement-list level: a
   [[fallthrough]]; nested one level deeper (inside the do/while or while
   bodies below) is not recognized as a valid marker, so those specific
   placement-constraint violations are not flagged -- the resulting
   fall-through is still reported at the next case label, though.
*/

void f(int n) {
    void g(void), h(void), i(void);
    switch (n) {
    case 1: /* diagnostic on fallthrough discouraged */
    case 2:
        g();
        [[fallthrough]];
    case 3: /* diagnostic on fallthrough discouraged */
        do {
            [[fallthrough]]; /* constraint violation: next statement is not
            part of the same secondary block execution */
        } while (false);
    case 6:
        do {
            [[fallthrough]]; /* constraint violation: next statement is not
            part of the same secondary block execution */
        } while (n--);
    case 7:
        while (false) {
            [[fallthrough]]; /* constraint violation: next statement is not
            part of the same secondary block execution */
        }
    case 5:
        h();
    case 4: /* fallthrough diagnostic encouraged */
        i();
        [[fallthrough]]; /* constraint violation */
    }
}
