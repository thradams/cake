/* CWE476 Juliet CWE476_NULL_Pointer_Dereference__binary_if_01 */

#pragma safety enable

void use(int i);

struct s { int a; };
void good()
{
    struct s* _Opt p = nullptr;
    if ((p != nullptr) && (p->a == 5)) { use(1); }
}
