int main()
{
  try
  {
     int i = 0;
     if (i == 0) return;    
     defer i = 1;    
     if (i == 0) return;     
  }
  catch
  {
     /*some error*/
  }
}

