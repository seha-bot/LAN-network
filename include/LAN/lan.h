#include<LAN/msg.h>
#include<winsock2.h>

struct network
{
    SOCKET socket;
    struct sockaddr_in address;
};

void createTCPNetwork(struct network *network, char *ip, u_short port)
{
    network->socket = socket(AF_INET, SOCK_STREAM, 0);

    network->address.sin_family = AF_INET;
    network->address.sin_addr.s_addr = inet_addr(ip);
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


void createUDPNetwork(struct network *network, char broadcast)
{
    network->socket = socket(AF_INET,SOCK_DGRAM,0);
    char mode = '1';
    setsockopt(network->socket,SOL_SOCKET,SO_BROADCAST,&mode,sizeof(mode));

    network->address.sin_family = AF_INET;
    network->address.sin_port = htons(8080);
    network->address.sin_addr.s_addr = broadcast ? INADDR_BROADCAST : INADDR_ANY;
}
void UDP_broadcast(char* buff, unsigned int size)
{
    struct network network;
    createUDPNetwork(&network, 1);
    sendto(network.socket, buff, size, 0, (SOCKADDR*)&network.address, sizeof(network.address));
    closesocket(network.socket);
}
void UDP_listen(char* buff, unsigned int size)
{
    struct network network;
    createUDPNetwork(&network, 0);
    bind(network.socket, (SOCKADDR*)&network.address, sizeof(network.address));

    unsigned long timeoutMs = 10;
    setsockopt(network.socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));
    recvfrom(network.socket, buff, size, 0, 0, 0);
    closesocket(network.socket);
}


void getIP(char* buff, unsigned int size)
{
    gethostname(buff, size);
    struct hostent *host = gethostbyname(buff);
    for (int i = 0; host->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
        char* ip = inet_ntoa(addr);
        memcpy(buff, ip, strlen(ip)+1);
    }
}
void IP_broadcast(char* hostname)
{
    char buff[512];
    char ip[256];

    getIP(ip, sizeof(ip));
    sprintf(buff, "%s:%s,", hostname, ip);
    UDP_broadcast(buff, 512);
}