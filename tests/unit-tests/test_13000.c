int main()
{
    1++;
    //error: lvalue required as increment operand [E122]
}

#pragma cake diagnostic check "-E122"
