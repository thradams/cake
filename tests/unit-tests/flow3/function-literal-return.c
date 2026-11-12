#pragma safety enable

void call(void (*f)(void));

int main(void)
{
    call((static void (void)) { return; });
    int i = 1; /* must not be reported as unreachable */
    return i;
}
