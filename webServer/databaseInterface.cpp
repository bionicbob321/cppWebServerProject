#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#endif

#include "databaseInterface.hpp"

#pragma comment(lib, "Ws2_32.lib")

using std::cout;
using std::cin;
using std::string;
using std::endl;

int main() {
    WSADATA wsaData;

    int iResult;

    //initialise winsock here
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    #define DEFAULT_PORT "27015"

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Resolve the local address and port to be used by the user
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    ///
    /// Server side socket listening
    ///

    //create new stuff
    SOCKET ListenSocket = INVALID_SOCKET;

    //Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    //check for errors to ensure a socket is a valid socket
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // to free memory allocated by getaddrinfo(), call freeaddrinfo(result);

    //Listening on a socket

    if (listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
        printf("Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    //Accepting a connection

    SOCKET ClientSocket;

    /* One programming technique is to create a continuous loop that checks for connection requests using the listen function (see Listening on a Socket).
     * If a connection request occurs, the application calls the accept, AcceptEx, or WSAAccept function and passes the work to another thread to handle the request.
     */

    //This example does not use multiple threads

    ClientSocket = INVALID_SOCKET;

    //Accept a client socket

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    //Receiving and sending data on the server

    #define DEFAULT_BUFLEN 512

    char recvbuf[DEFAULT_BUFLEN];
    int iSendResult;
    iResult = 0;
    int recvbuflen = DEFAULT_BUFLEN;

    //Receive until the peer shuts down the connection
    do { //Do/While will always run code once then checks if it should be run again

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) { //This is where to deal with the data once it is recieved. this if statement is run when data is recieved
            printf("Bytes recieved: %d\n", iResult);
            //std::cout << recvbuf << std::endl; //recvbug contains the message from the client
            handleRequest(recvbuf); //handles request of login data
            //need to add ability to update records

            //Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if(iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup;
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        } else if (iResult == 0) {
            printf("Connection closing...\n");
        } else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);
}