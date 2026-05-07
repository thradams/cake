int main()
{
    goto a;
    a:;

    //error: duplicated label 'a'

    a:; //lint 1440
} 


