/* CWE476 Juliet CWE476_NULL_Pointer_Dereference__char_01 */

#pragma safety enable

void use(int i);

void bad()
{
    char* _Nullable p = nullptr;
    use(p[0]);
}
