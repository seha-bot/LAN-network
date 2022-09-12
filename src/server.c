#include <windows.h>
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2.lib")
 
/// Set device IP address and port number
#define DEVICE_IP_ADDRESS   "192.168.2.3"
#define PORT_NUMBER         23
#define BUFFER_SIZE         255
 
int main()
{
    WSADATA ws;            ///< Structure to contain information about the Windows Socket implementation
    DWORD retVal = 0;
    DWORD choice = 0;
    BOOL loopCounter = TRUE;
    SOCKET listeningSocket;
    SOCKET commsocket;
    CHAR sendbuffer[BUFFER_SIZE] = {0};
    CHAR recvbuffer[BUFFER_SIZE] = {0};
    struct sockaddr_in serverinfo;
 
    printf("Demo Program to send and receive data over LAN, Toradex Module as server\n");
    retVal = WSAStartup(0x0101, &ws);                                          ///< Initiating use of ws2.dll
    if (retVal != 0)                                                           ///< If WSAStartup failed to initialize
    {
        printf("WSAStartup failed with error code: %d\n", retVal);
        WSACleanup();
        exit(1);
    }
 
    listeningSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);                 ///< Socket creation
    if (listeningSocket == INVALID_SOCKET)
    {
        printf("\nSocket creation failed with error code : %d", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
 
    /// Socket binding
    serverinfo.sin_family = AF_INET;                                           ///< TCP/UDP socket
    serverinfo.sin_addr.s_addr = inet_addr(DEVICE_IP_ADDRESS);                 ///< IP Address of Toradex module
    serverinfo.sin_port = htons(PORT_NUMBER);                                  ///< Port number: used for communication 
 
    retVal = bind(listeningSocket, (SOCKADDR *)&serverinfo, sizeof(struct sockaddr));
    if (retVal == SOCKET_ERROR)
    {
        printf("\nSocket binding failed with error code : %d", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
 
    retVal = listen(listeningSocket, 1);                                        ///< Listen for 1 socket only
    if (retVal == SOCKET_ERROR)
    {
        printf("\nSocket listening failed with error code : %d", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
 
    printf("\nWaiting for client to respond....");
    commsocket = accept(listeningSocket, NULL, NULL);                           ///< Waiting for request from client
    if (commsocket == INVALID_SOCKET) 
    {
        printf("\nConnection failed with error code : %d", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
 
    while (loopCounter == TRUE)
    {
        printf("\n1. Send\n2. Receive\n3. Exit\n");
        scanf("%d", &choice);
        switch(choice)
        {
        case 1:
            printf("Send Message = ");
            scanf("%s", sendbuffer);
            retVal = send(commsocket, sendbuffer, strlen(sendbuffer), 0);       ///< Sending to client
            if (retVal == SOCKET_ERROR)
            {
                printf("\nFailed to receive from client with error code : %d", WSAGetLastError()); 
            }
            break;
        case 2:
            printf("\nListening socket...");
            retVal = recv(commsocket, recvbuffer, BUFFER_SIZE, 0);              ///< Receiving from client
                recvbuffer[retVal] = 0;
                if (retVal == SOCKET_ERROR)
                {
                    printf("\nFailed to receive from client with error code : %d", WSAGetLastError());
                }
                else
                {
                    printf("\nMessage from Client : %s", recvbuffer);
                }
            break;
        case 3:
            loopCounter = FALSE;
            break;
        default:
            printf("Invalid choice \n");
            break;
        }
    }
    closesocket(commsocket);                                                    ///< Closing communication socket 
    closesocket(listeningSocket);                                               ///< Closing listening socket
    WSACleanup();                                                               ///< Releasing ws2.dll
    return TRUE;
}