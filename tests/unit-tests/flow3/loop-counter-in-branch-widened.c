#pragma safety enable

/*
  A counter incremented under a condition in a loop can pass any bound:
  `origins > 8` is reachable. Reduced from flow_alternative.c
  flow_alternatives_remove_duplicates.
*/

void use(int);

void f(int n, const _Bool * dup)
{
    int origins = 0;
    for (int k = 0; k < n; k++)
    {
        if (!dup[k])
            origins++;
    }
    if (origins > 8)
    {
        use(origins);
    }
}
