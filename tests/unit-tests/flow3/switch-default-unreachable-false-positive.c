#pragma safety enable

/* `default:` gets its own child map like `case N:`, so a `break` in it no longer marks the pre-switch map dead and the code after the switch is reachable */

int use(int e)
{
    switch (e)
    {
        case 1:
            break;
        case 2:
            break;
        default:
            break;
    }
    return 0; /* ok: reachable, must NOT warn "unreachable code" */
}

/* contrast: real dead code after `break;` inside the same case is still flagged */
void real_unreachable(int e)
{
    switch (e)
    {
        default:
            break;
            int x = 1; //lint 68 unreachable code
            (void)x;
    }
}
