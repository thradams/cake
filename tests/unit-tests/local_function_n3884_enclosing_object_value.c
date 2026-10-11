/* N3884: an automatic object of the enclosing function can be used only in unevaluated operands */

int main()
{
    int i;
    static void local()
    {
        typeof(i) j = 0;    /* valid */
        auto k = sizeof(i); /* valid */
        int n = i;          //lint 1870
    }
}
