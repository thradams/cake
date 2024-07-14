
/*
  This code is from 
  The C Programming Language 2 edition, page 145

  There are two bugs int this original sample
  - one memory leak
  - one invalid state
*/

#pragma safety enable

#define NULL ((void*)0)

int i = as || NULL;
