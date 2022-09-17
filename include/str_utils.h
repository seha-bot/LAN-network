#ifndef STR_UTILS
#define STR_UTILS

char str_upper(char* s)
{
    int sc = 0;
    while(s[sc] != '\0')
    {
        if(s[sc] >= 'a' && s[sc] <= 'z') s[sc] -= 'a'-'A';
        sc++;
    }
}
char str_lower(char* s)
{
    int sc = 0;
    while(s[sc] != '\0')
    {
        if(s[sc] >= 'A' && s[sc] <= 'Z') s[sc] += 'a'-'A';
        sc++;
    }
}

#define swap(a, b) (a += b, b = a - b, a -= b)
void itos(char* s, int v)
{
    int c = 0;
    if(v == 0) s[c++] = '0';
    else
    {
        while(v > 0)
        {
            s[c++] = '0' + (v % 10);
            v *= 0.1;
        }
        for(int i = 0; i < (c>>1); i++) swap(s[i], s[c-i-1]);
    }
    s[c++] = '\0';
}

#endif
