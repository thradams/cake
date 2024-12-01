int main()
{
  do
  {
     int i;
     defer i = 1;     
     if (i) break;
     defer i = 3;     
  }
  while(0);
}

