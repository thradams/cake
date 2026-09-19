#pragma safety enable
#define NULL ((void*)0)

/* an assignment used as an operand, `(dp = readdir(dir)) != NULL`, must narrow dp inside the loop */

struct dirent { int d; };
struct DIR { int x; };
struct dirent* _Opt readdir(struct DIR* dir);

void use_while_loop(struct DIR* dir)
{
    struct dirent* _Opt dp;
    while ((dp = readdir(dir)) != NULL)
    {
        int x = dp->d; /* ok: dp is non-null here */
    }
}

/* contrast: a plain assignment with no null check is still flagged */
void use_no_check(struct DIR* dir)
{
    struct dirent* _Opt dp;
    dp = readdir(dir);
    int x = dp->d; //lint 33 -> operator applied to a null pointer
}

/* contrast: after the loop dp can be null, still flagged */
void use_after_loop(struct DIR* dir)
{
    struct dirent* _Opt dp;
    while ((dp = readdir(dir)) != NULL)
    {
    }
    int x = dp->d; //lint 33 -> operator applied to a null pointer
}
