#pragma safety enable

/* `if (f1() && f1()) { return 1; } return 0;` must not report the last return unreachable: an arm aliasing the pre-branch map is wrapped in a fresh child (enable_vt_mode) */

int f1(void);

int use(void)
{
    if (f1() && f1())
    {
        return 1;
    }
    return 0; /* ok: reachable, must NOT warn "unreachable code" */
}

int use3(void)
{
    if (f1() && f1() && f1())
    {
        return 1;
    }
    return 0; /* ok: same false positive, one more && link */
}

/* contrast: real dead code after an unconditional jump is still flagged */
void real_unreachable(void)
{
    return;
    int x = 1; //lint 68 unreachable code
    (void)x;
}
