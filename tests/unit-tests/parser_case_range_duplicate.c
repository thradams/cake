/*
   Crash regression: case_label_list_find_range's single-value branch
   ("if (p->constant_expression_end == NULL)") dereferenced
   p->constant_expression_end->object -- the very field it just checked
   was NULL. Any switch with a case range ("case A ... B:") checked
   against an earlier single-value case crashed unconditionally.

   It also never guarded against a `default:` label reaching the same
   branch (default has constant_expression == NULL too, pushed into the
   same case_label_list by case_label_list_push), a second, independent
   NULL-deref path once the first one was fixed.
*/

int main()
{
    int x = 3;
    switch (x)
    {
        default:
            break;
        case 100:
            break;
        case 1 ... 5:
            break;
    }
}
