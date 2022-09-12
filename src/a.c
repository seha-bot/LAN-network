#include<stdio.h>
#include<winsock2.h>
#include<gl_utils.h>

#define WIDTH 250
#define HEIGHT 250
#define HW 125
#define HH 125

struct network
{
    u_int socket;
    struct sockaddr_in address;
};

void createNetwork(struct network *network, u_short port)
{
    network->socket = socket(AF_INET, SOCK_STREAM, 0);
    network->address.sin_family = AF_INET;

    network->address.sin_addr.s_addr = inet_addr("192.168.196.22");
    network->address.sin_port = htons(port);
}

void host(struct network *network)
{
    bind(network->socket, (struct sockaddr *) &network->address, sizeof(network->address));
    listen(network->socket, SOMAXCONN);
}
void join(struct network *network)
{
    connect(network->socket, (struct sockaddr *) &network->address, sizeof(network->address));
}

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

char connection = 0; // 0 - undefined, 1 - host, 2 - join
struct network network;
int client_socket = 0;
void h()
{
    printf("HOST\n");
    createNetwork(&network, 8080);
    host(&network);
    client_socket = accept(network.socket, NULL, NULL);
    printf("accepted\n");
    connection = 1;
}
void j()
{
    printf("JOIN\n");
    createNetwork(&network, 8080);
    join(&network);
    connection = 2;
}

char msg[256];
int len = 0;
char wall[10][256];
int wallLen = 0;
void s()
{
    printf("SENT\n");
    send(connection == 1 ? client_socket : network.socket, msg, sizeof(msg), 0);
    sprintf(wall[wallLen++], "YOU = %s", msg);
    len = 0;
    msg[len] = '\0';
}

void r(unsigned int socket)
{
    unsigned long timeoutMs = 10;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));
    char msga[256];
    int rec = recv(socket, msga, sizeof(msga), 0);
    if(rec == 256)
    {
        sprintf(wall[wallLen++], "THEM = %s", msga);
    }
}

int loop()
{
    if(!connection)
    {
        button("HOST", h, HW-20, HH, 40, 20);
        button("JOIN", j, HW-20, HH-30, 40, 20);
    }
    else
    {
        drawString(msg, 0, len > 35 ? 8 : 0, 1);
        button("SEND", s, WIDTH-40, 0, 40, 16);
        line(0, 16, WIDTH, 16, 1);
        for(int i = 0; i < wallLen; i++)
        {
            drawString(wall[i], 0, HEIGHT-(i+1)*8, 1);
        }
        r(connection == 1 ? client_socket : network.socket);
    }
    if(keys)
    {   
        if(keys[0] == GLFW_KEY_ENTER)
        {
            s();
            return 0;
        }
        if(keys[0] != GLFW_KEY_BACKSPACE)
        {
            msg[len++] = keys[0];
            if(len == 35) msg[len++] = '\n';
        }
        else len--;
        len = clamp(len, 0, 71);
        msg[len] = '\0';
    }
    return 0;
}

void getIP()
{
    char buff[200];
    gethostname(buff, sizeof(buff));
    printf("%s\n", buff);
    struct hostent *phe = gethostbyname(buff);
    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        printf("Address %d: %s\n", i, inet_ntoa(addr));
    }
}

void broadcastIP()
{
    SOCKET sock;
    sock = socket(AF_INET,SOCK_DGRAM,0);
    char broadcast = '1';
    setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast));
    struct sockaddr_in Recv_addr;
    int len = sizeof(struct sockaddr_in);
    char sendMSG[] ="Broadcast message from SLAVE TAG";
    char recvbuff[50] = "";
    int recvbufflen = 50;
    Recv_addr.sin_family       = AF_INET;
    Recv_addr.sin_port         = htons(8080);
    Recv_addr.sin_addr.s_addr  = INADDR_BROADCAST;
    sendto(sock,sendMSG,strlen(sendMSG)+1,0,(SOCKADDR *)&Recv_addr,sizeof(Recv_addr));
    printf("%s\n", recvbuff);
    closesocket(sock);
}

int main()
{
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Winsock Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    
    // createWindow("Window", WIDTH<<1, HEIGHT<<1);
    // if(!window) return 1;
    // glPointSize(2.0);
    
    // start(loop);
    // return 0;

    // broadcastIP();
    // return 0;

    SOCKET sock;
    sock = socket(AF_INET,SOCK_DGRAM,0);
    char broadcast = '1';
    setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast));
    struct sockaddr_in Recv_addr;
    struct sockaddr_in Sender_addr;
    int len = sizeof(struct sockaddr_in);
    char recvbuff[50];
    int recvbufflen = 50;

    Recv_addr.sin_family       = AF_INET;        
    Recv_addr.sin_port         = htons(8080);   
    Recv_addr.sin_addr.s_addr  = INADDR_ANY;
    bind(sock,(SOCKADDR*)&Recv_addr, sizeof(Recv_addr));

    recvfrom(sock,recvbuff,recvbufflen,0,(SOCKADDR*)&Sender_addr,&len);
    printf("\n\n\tReceived Message is : %s", recvbuff);
    closesocket(sock);
}
