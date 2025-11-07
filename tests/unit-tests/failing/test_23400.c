#pragma safety enable

int main()
{
     for (int i = 0; i < 10; i++)
     {
       if (i != 0) //we dont have warning i always zero here
       {
       }
     }
     int k = 0;
     k--;
     static_state(k, "not-zero");
}