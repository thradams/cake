static inline unsigned short SDL_Swap16(unsigned short x)
{
  __asm__("xchgb %b0,%h0": "=abcd"(x):"0"(x));
    return x;
}

int main()
{
    SDL_Swap16(1);
}