#pragma safety enable

int main()
{
     int k = 0;
     k--;
     static_state(k, "not-zero");
}