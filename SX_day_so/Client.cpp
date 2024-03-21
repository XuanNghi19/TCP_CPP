#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    struct addrinfo* result = nullptr;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo("localhost", "8080", &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed: " << iResult << endl;
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        cout << "Unable to connect to server: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Unable to connect to server!" << endl;
        WSACleanup();
        return 1;
    }

    string data;

    cout << "Nhap day so: ";
    getline(cin, data);
    data += "\0";
    iResult = send(ConnectSocket, data.c_str(), data.length() + 1, 0);
    if (iResult == SOCKET_ERROR) {
        cout << "send failed: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Sent data to server: " << data << endl;

    char recvbuf[1025];
    int recvbuflen = 1024;
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) { 
        recvbuf[iResult] = '\0';
        cout << "Received from server: " << recvbuf << endl;
    } else if (iResult == 0) {
        cout << "Connection closed" << endl;
    } else {
        cout << "recv failed: " << WSAGetLastError() << endl;
    }

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}