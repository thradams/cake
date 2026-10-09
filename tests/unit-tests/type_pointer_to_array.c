#pragma default_null

void F(char(*p)[10])
{
    (*p)[0] = 'a';
}

