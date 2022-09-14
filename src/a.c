#include<stdio.h>
#include<math.h>
#include<gl_utils.h>
#include<LAN/lan.h>

#define WIDTH 250
#define HEIGHT 250
#define HW 125
#define HH 125

void button(char* s, void(*f)(), int x, int y, int w, int h)
{
    int mx = mousePos[0] * 0.5f;
    int my = ((HEIGHT - mousePos[1]) + HEIGHT) * 0.5f;

    char c = 1;
    if(x < mx && mx < x+w && y < my && my < y+h)
    {
        c = 2;
        if(getMouseButtonDown(GLFW_MOUSE_BUTTON_1)) f();
    }

    for(int i = x; i < x+w; i++)
    {
        for(int j = y; j < y+h; j++)
        {
            pixel(i, j, c);
        }
    }
    float l = strlen(s);
    float tw = (6.0f * l - 2.0f) * 0.5f;
    float th = 8.0f*0.5f;
    drawString(s, x+(w>>1)-tw, y+(h>>1)-th, 0);
}




char connection = 0; // 0 - undefined, 1 - lobby, 2 - awdiohpsiod
struct network network;
int client_socket = 0;
void h()
{
    printf("HOST\n");
    connection = 1;
}
void j()
{
    printf("JOIN\n");
    connection = 2;
}

char buff[256];
char servers[10][2][2][256];
int loop()
{
    switch (connection)
    {
    case 0:
        /*MODE SELECT*/
        button("HOST", h, HW-20, HH, 40, 20);
        button("JOIN", j, HW-20, HH-30, 40, 20);
        break;
    case 1:
        /*LOBBY HOST*/
        IP_broadcast();
        line(0,0,WIDTH,HEIGHT,(int)((sin(glfwGetTime() * 10000.0) + 1) * 100.0) % 8 + 1);
        break;
    case 2:
        /*LOBBY JOIN*/
        IP_listen(buff, sizeof(buff));
        char server[2][2][256];
        msg_decode(buff, server);
        int e1 = msg_find("name", server, 2);
        int e2 = msg_find("ip", server, 2);
        if(e1 >= 0 && e2 >= 0)
        {
            printf("HOST NAME = %s\n", server[e1][1]);
            printf("IP = %s\n", server[e2][1]);
        }
        // printf("%s\n", buff);
        line(0,0,WIDTH,HEIGHT,(int)((sin(glfwGetTime() * 10000.0) + 1) * 100.0) % 8 + 1);
        break;
    // case 1912:
    //     drawString(msg, 0, len > 35 ? 8 : 0, 1);
    //     button("SEND", s, WIDTH-40, 0, 40, 16);
    //     line(0, 16, WIDTH, 16, 1);
    //     for(int i = 0; i < wallLen; i++)
    //     {
    //         drawString(wall[i], 0, HEIGHT-(i+1)*8, 1);
    //     }
    //     r(connection == 1 ? client_socket : network.socket);
    //     break;
    }
    return 0;
}

int main(int argc, char** argv)
{
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Winsock Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    
    createWindow("Window", WIDTH<<1, HEIGHT<<1);
    if(!window) return 1;
    glPointSize(2.0);
    
    start(loop);
    return 0;
}


/*
TEST (DELETE)

    char msg[256] = "name1:val1,name2:val2,";
    char map[2][2][256] = {
        {"name1", "val1"},
        {"name2", "val2"},
    };
    char msgout[256];
    char mapout[2][2][256];
    msg_decode(msg, mapout);
    msg_encode(map, 2, msgout);
    printf("%s\n", msgout);
    printf("%s\n", map[0][0]);
    printf("%s\n", map[0][1]);
    printf("%s\n", map[1][0]);
    printf("%s\n", map[1][1]);
*/