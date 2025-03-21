#pragma safety enable
struct X {
  char * _Owner _Opt text;
};

void init([[dtor]] struct X *p)
{
  //p->text = 0;  
}