/* CWE476 Juliet CWE476_NULL_Pointer_Dereference__int_01 */

#pragma safety enable

void use(int i);

void good()
{
    int* _Nullable p = nullptr;
    if (p != nullptr) { use(*p); }
}
