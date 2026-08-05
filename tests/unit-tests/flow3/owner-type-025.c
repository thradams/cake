struct X { const char* _Owner text; };
void destroyX(struct X x) 
{
	runtime_assert(x.text == 0);
}

int main() 
{
}
