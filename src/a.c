#include<stdio.h>
#include<math.h>
#include<gl_utils.h>
#include<LAN/lan.h>

#define WIDTH 250
#define HEIGHT 250
#define HW 125
#define HH 125

void textBox(char* s, int x, int y, int w, int h, char c)
{
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

    textBox(s, x, y, w, h, c);
}

void box(int x, int y, int w, int h, char c)
{
    for(int i = x; i < x+w; i++)
    {
        for(int j = y; j < y+h; j++)
        {
            pixel(i, j, c);
        }
    }
}

char connection = 0;
char hostname[256];
struct network network;
char buff[256];
char servers[5][2][256]; //TODO: replace with nec
int server_c = 0;

int p1 = HH-20, p2 = HH-20, b = HW | (HH<<16), dir = 0x00020002, score = 0;
double update = 0.0;
char isHost = 0;
void play()
{
    p2 = atoi(buff);
    if(!isHost)
    {
        b = atoi(buff + 33);
        score = atoi(buff + 66);
    }
    if(getKey(GLFW_KEY_W)) p1 += 100*dt;
    if(getKey(GLFW_KEY_S)) p1 -= 50*dt;

    p1 = clamp(p1, 0, HEIGHT-40);

    box(15, p1, 5, 40, 1);
    box(WIDTH-20, p2, 5, 40, 1);
    

    int bx = 0xFFFF & b;
    int by = b>>16;
    
    int dx = 0xFFFF & dir;
    int dy = dir>>16;

    int s1 = 0xFF & score;
    int s2 = score>>8;
    if(s1 == 5 || s2 == 5) connection = 4;
    
    drawChar('0'+s1, HW-20, HH-8, 1);
    drawChar('0'+s2, HW+20, HH-8, 1);
    if(isHost)
    {
        if(!update)
        {
            bx += (dx-1) * 2;
            by += (dy-1) * 2;
        }

        if(bx < 22 && by > p1 && by < p1+40) dx = 2;
        if(bx > WIDTH-32 && by > p2 && by < p2+40) dx = 0;

        if(bx < 0) dx = 2, bx = 0, s2++;
        if(bx > WIDTH-10)  dx = 0, bx = WIDTH-10, s1++;
        if(by < 0) dy = 2, by = 0;
        if(by > HEIGHT-10) dy = 0, by = HEIGHT-10;
        box(bx, by, 10, 10, 2);
    }
    else box(WIDTH-bx-10, by, 10, 10, 2);

    b = bx | (by<<16);
    dir = dx | (dy<<16);
    score = s1 | (s2<<8);
}

void btn_host() { connection = 1; }
void btn_join() { connection = 2; }
void btn_request_connection()
{
    createTCPNetwork(&network, buff, 8080);
    sprintf(buff, "%s:request,", buff);
    for(int i = 0; i < 300; i++) UDP_broadcast(buff, sizeof(buff));
    join(&network);
    connection = 3;
};
void btn_refresh_servers() { server_c = 0; }

int loop()
{
    update += dt;
    if(update >= 0.01) update = 0.0;
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
            if(!strcmp(request[1], "request"))
            {
                getIP(buff, sizeof(buff));
                if(!strcmp(request[0], buff))
                {
                    createTCPNetwork(&network, buff, 8080);
                    host(&network);
                    network.socket = accept(network.socket, NULL, NULL);
                    connection = 3;
                    isHost = 1;
                }
            }
        }
        IP_broadcast(hostname);
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
                if(!strcmp(servers[i][1], server[1]))
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
        break;
    case 3:
        /*PLAY SCREEN*/
        itos(buff, p1);
        if(isHost)
        {
            itos(buff+33, b);
            int s1 = 0xFF & score;
            int s2 = score>>8;
            int subswap = s2 | (s1<<8);
            itos(buff+66, subswap);
        }
        Treba_Mi_Ime_Za_Ovu_Funkciju(network.socket, buff, sizeof(buff), buff, sizeof(buff));
        play();
        break;
    case 4:
        /*GAME OVER*/
        ;int s1 = 0xFF & score;
        int s2 = score>>8;
        if(s1 > s2) textBox("YOU HAVE WON", 0, HH-15, WIDTH, 30, 1);
        else textBox("YOU HAVE LOST", 0, HH-15, WIDTH, 30, 1);
        break;
    }
    return 0;
}

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
    // srand(time(0));
    // hostname[0] = rand()%255;
    // hostname[1] = rand()%255;
    // hostname[2] = '\0';
    
    createWindow("Window", WIDTH<<1, HEIGHT<<1);
    if(!window) return 1;
    glPointSize(2.0);
    
    start(loop);
    return 0;
}
