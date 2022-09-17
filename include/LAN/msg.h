#ifndef SEHA_MSG
#define SEHA_MSG
#include<stdio.h>
#include<string.h>

/*TODO: Make actual maps with hashing for speed*/
/*
MSG FORMAT:
name1:val1,name2:val2,

MAP FORMAT:
char map[n][2][256] = {
    {"name1", "val1"},
    {"name2", "val2"},
    ...
};
*/

void msg_decode(char* msg, char out[][2][256])
{
    char mode = 0; //0:name,1:val
    int c = 0, i = 0, o = 0;
    while(msg[c] != '\0')
    {
        if(msg[c] == ':' || msg[c] == ',')
        {
            out[i][mode][c - o] = '\0';
            o = c + 1;
            i += mode;
            mode = !mode;
        }
        else out[i][mode][c - o] = msg[c];
        c++;
    }
}

void msg_encode(char map[][2][256], unsigned int size, char* out)
{
    int c = 0;
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            int mc = 0;
            while(map[i][j][mc] != '\0') out[c++] = map[i][j][mc++];
            out[c++] = j ? ',' : ':';
        }
    }
    out[c] = '\0';
}

/*TODO: BELOW FUNCTIONS ARE UNTESTED*/

int msg_find(char* key, char map[][2][256], unsigned int size)
{
    for(int i = 0; i < size; i++) if(strcmp(map[i][0], key)) return i;
    return -1;
}
#endif
