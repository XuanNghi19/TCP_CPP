#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void Sx()
{
}

int main()
{
  WSADATA wsaData;
  SOCKET listening = INVALID_SOCKET;
  SOCKET client = INVALID_SOCKET;

  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0)
  {
    cout << "WSAStartup failed: " << iResult << endl;
    return 1;
  }

  struct addrinfo *result = nullptr;
  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  iResult = getaddrinfo(nullptr, "8080", &hints, &result);
  if (iResult != 0)
  {
    cout << "getaddrinfo failed: " << iResult << endl;
    WSACleanup();
    return 1;
  }

  listening = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (listening == INVALID_SOCKET)
  {
    cout << "Error at socket(): " << WSAGetLastError() << endl;
    freeaddrinfo(result);
    WSACleanup();
    return 1;
  }

  iResult = bind(listening, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR)
  {
    cout << "bind failed with error: " << WSAGetLastError() << endl;
    freeaddrinfo(result);
    closesocket(listening);
    WSACleanup();
    return 1;
  }

  freeaddrinfo(result);

  if (listen(listening, SOMAXCONN) == SOCKET_ERROR)
  {
    cout << "Listen failed with error: " << WSAGetLastError() << endl;
    closesocket(listening);
    WSACleanup();
    return 1;
  }

  cout << "Server started, listening on port 8080..." << endl;

  int i = 1;
  while (true)
  {
    client = INVALID_SOCKET;
    client = accept(listening, nullptr, nullptr);
    if (client == INVALID_SOCKET)
    {
      cout << "accept failed: " << WSAGetLastError() << endl;
      closesocket(listening);
      WSACleanup();
      return 1;
    }

    cout << "_______________________Stages " << i << "_______________________" << endl;
    cout << "Client connected!" << endl;
    char recvbuf[1025];
    int recvbuflen = 1024;
    iResult = recv(client, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
      recvbuf[iResult] = '\0';
      // Chuyển đổi chuỗi nhận được thành mảng các số
      vector<int> numbers;
      stringstream ss(recvbuf);
      int num;
      while (ss >> num)
      {
        numbers.push_back(num);
      }

      // Sắp xếp mảng các số theo thứ tự tăng dần
      sort(numbers.begin(), numbers.end());
      cout << "Day so da duoc sap xep!" << endl;

      // Chuyển đổi mảng các số đã sắp xếp thành chuỗi
      stringstream sorted_ss;
      for (int n : numbers)
      {
        sorted_ss << n << " ";
      }

      string sorted_string = sorted_ss.str() + "\0";

      // Gửi chuỗi đã sắp xếp lại cho client
      iResult = send(client, sorted_string.c_str(), sorted_string.length(), 0);
      if (iResult == SOCKET_ERROR)
      {
        cout << "send failed: " << WSAGetLastError() << endl;
        closesocket(client);
        WSACleanup();
        return 1;
      }
    }
    else if (iResult == 0)
    {
      cout << "Connection closed" << endl;
    }
    else
    {
      cout << "recv failed: " << WSAGetLastError() << endl;
    }
    closesocket(client);

    cout << "_______________________End Stages_______________________\n\n\n";
    i++;
  }

  closesocket(listening);
  WSACleanup();

  return 0;
}