#pragma safety enable

/* if (T x = ...) with break, continue or throw in its body: the join maps must not keep a key that dies with the if */

struct node { struct node* _Opt next; int value; };
struct node* _Opt get();

void break_in_body(int n)
{
    for (int i = 0; i < n; i++)
    {
        if (struct node* _Opt p = get())
        {
            (void)p->value;
            break;
        }
    }
}

void continue_in_body(int n)
{
    for (int i = 0; i < n; i++)
    {
        if (struct node* _Opt p = get())
        {
            (void)p->value;
            continue;
        }
    }
}

void throw_in_body(void)
{
    try
    {
        if (struct node* _Opt p = get())
        {
            (void)p->value;
            throw;
        }
    }
    catch
    {
    }
}

void break_and_continue_arms(int n)
{
    while (n--)
    {
        if (struct node* _Opt p = get())
            break;
        else
            continue;
    }
}
