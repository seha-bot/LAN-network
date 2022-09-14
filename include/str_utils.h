#ifndef STR_UTILS
#define STR_UTILS

char str_comp(char* s1, char* s2)
{
    int s1c = 0;
    int s2c = 0;
    while(1)
    {
        if(s1[s1c] == '\0' && s2[s2c] != '\0' || s1[s1c] != '\0' && s2[s2c] == '\0') return 0;
        if(s1[s1c] == '\0' && s2[s2c] == '\0') return 1;
        if(s1[s1c++] != s2[s2c++]) break;
    }
    return 0;
}

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

#endif
