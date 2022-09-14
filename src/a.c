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
char hostname[256];
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
char servers[5][2][256]; //TODO: replace with nec
int server_c = 0;
int loop()
{
    switch(connection)
    {
    case 0:
        /*MODE SELECT*/
        button("HOST", h, HW-20, HH, 40, 20);
        button("JOIN", j, HW-20, HH-30, 40, 20);
        break;
    case 1:
        /*LOBBY HOST*/
        IP_broadcast(hostname);
        line(0,0,WIDTH,HEIGHT,(int)((sin(glfwGetTime() * 10000.0) + 1) * 100.0) % 8 + 1);
        break;
    case 2:
        /*LOBBY JOIN*/
        buff[0]=0;
        IP_listen(buff, sizeof(buff));
        char server[2][256];
        msg_decode(buff, &server);
        if(strlen(buff))
        {
            char contains = 0;
            for(int i = 0; i < server_c; i++)
            {
                if(str_comp(servers[i][1], server[1]))
                {
                    contains = 1;
                    break;
                }
            }
            if(!contains && server_c < 5) memcpy(servers[server_c++], server, sizeof(server));
        }
        for(int i = 0; i < server_c; i++)
        {
            void a(){printf("hello %d\n", i);};
            button(servers[i][0], a, HW-50, HEIGHT - i * 15 - (i+2) * 15, 100, 15);
        }
        void refresh(){server_c = 0;};
        button("REFRESH", refresh, HW-30, HEIGHT - server_c * 15 - (server_c+2) * 15, 60, 15);
        break;
    }
    return 0;
}

#include<str_utils.h>
int main(int argc, char** argv)
{
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Winsock Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }

    printf("My name is: ");
    scanf("%s", hostname);
    str_upper(hostname);
    
    createWindow("Window", WIDTH<<1, HEIGHT<<1);
    if(!window) return 1;
    glPointSize(2.0);
    
    start(loop);
    return 0;
}
