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

char connection = 0;
char hostname[256];
struct network network;
int client_socket = 0;
char buff[256];
char servers[5][2][256]; //TODO: replace with nec
int server_c = 0;

void Treba_Mi_Ime_Za_Ovu_Funkciju(SOCKET socket, char* inBuff, unsigned int inSize, char* outBuff, unsigned int outSize)
{
    if(inSize)
    {
        unsigned long timeoutMs = 10;
        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));
        int rec = recv(socket, inBuff, inSize, 0);
        if(rec == inSize)
        {
            printf("%s\n", inBuff);
        }
    }
    if(outSize) send(socket, outBuff, outSize, 0);
}

void btn_host() { connection = 1; }
void btn_join() { connection = 2; }
void btn_request_connection()
{
    createTCPNetwork(&network, buff, 8080);
    sprintf(buff, "%s:request,", buff);
    for(int i = 0; i < 300; i++) UDP_broadcast(buff, sizeof(buff));
    join(&network);
};
void btn_refresh_servers() { server_c = 0; }

int loop()
{
    switch(connection)
    {
    case 0:
        /*MODE SELECT*/
        button("HOST", btn_host, HW-20, HH, 40, 20);
        button("JOIN", btn_join, HW-20, HH-30, 40, 20);
        break;
    case 1:
        /*LOBBY HOST*/
        buff[0]=0;
        UDP_listen(buff, sizeof(buff));
        if(strlen(buff))
        {
            char request[2][256];
            msg_decode(buff, &request);
            if(str_comp(request[1], "request"))
            {
                getIP(buff, sizeof(buff));
                if(str_comp(request[0], buff))
                {
                    createTCPNetwork(&network, buff, 8080);
                    host(&network);
                    client_socket = accept(network.socket, NULL, NULL);
                }
            }
        }
        IP_broadcast(hostname);
        
        if(client_socket)
        {
            buff[0] = 'H';
            buff[1] = 'I';
            buff[2] = '\0';
            Treba_Mi_Ime_Za_Ovu_Funkciju(client_socket, 0, 0, buff, sizeof(buff));
        }

        line(0,0,WIDTH,HEIGHT,(int)((sin(glfwGetTime() * 10000.0) + 1) * 100.0) % 8 + 1);
        break;
    case 2:
        /*LOBBY JOIN*/
        buff[0]=0;
        UDP_listen(buff, sizeof(buff));
        if(strlen(buff))
        {
            char server[2][256];
            msg_decode(buff, &server);
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
            memcpy(buff, servers[i][1], 256);
            button(servers[i][0], btn_request_connection, HW-50, HEIGHT - i * 15 - (i+2) * 15, 100, 15);
        }
        button("REFRESH", btn_refresh_servers, HW-30, HEIGHT - server_c * 15 - (server_c+2) * 15, 60, 15);
        Treba_Mi_Ime_Za_Ovu_Funkciju(network.socket, buff, sizeof(buff), 0, 0);
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
