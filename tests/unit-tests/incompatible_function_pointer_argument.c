/*
   C23 6.7.6.3p13: a parameter declared as "function returning T" is
   adjusted to "pointer to function returning T".

   Cake does not do that adjustment, so in check_assigment the parameter
   type stays TYPE_CATEGORY_FUNCTION, the
   'type_is_pointer_or_array(p_a_type)' branch is not taken and the
   type_is_compatible check is never reached.
*/

void login_async_ptr(void * data, void (*callback)(void * data, int id));
void login_async_fn(void * data, void callback(void * data, int id));

static void callback_swapped(int id, void * capture)
{
}

int main()
{
    /* works: parameter written as a pointer to function */
    login_async_ptr(0, callback_swapped); //lint 54 incompatible types

    /* bug: same parameter written as a function declarator, not diagnosed */
    login_async_fn(0, callback_swapped); //lint 54 incompatible types

    return 0;
}
