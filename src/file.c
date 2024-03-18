
int main()
{
    int j;
    int i;
    switch (i)
    {
        case 1:
            static_state(j);
            break;
        case 2:
            static_debug(j);
            break;
        default:
            j = 0;
            break;
    }
    static_debug(j);
}
