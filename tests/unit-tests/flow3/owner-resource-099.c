#pragma safety enable

//warning: object pointed by '?' was not released.

void free(void* _Owner) // owner object () not moved (root see line 5)
{

} //lint 29 owner object () not moved (root see line 5)
