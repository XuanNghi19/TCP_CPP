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

struct SinhVien
{
  string maSV;
  string hoTen;
  string soDT;
  string email;
};

vector<SinhVien> danhSachSV;

void LoadDuLieuSV()
{
  ifstream file("sv.txt");
  string line;
  while (getline(file, line))
  {
    istringstream iss(line);
    string token;
    SinhVien sv;
    getline(iss, token, '#');
    sv.maSV = token;
    getline(iss, token, '#');
    sv.hoTen = token;
    getline(iss, token, '#');
    sv.soDT = token;
    getline(iss, token, '#');
    sv.email = token;
    danhSachSV.push_back(sv);
  }
  file.close();
}

void LuuDuLieuSV()
{
  ofstream file("sv.txt");
  for (const auto &sv : danhSachSV)
  {
    file << sv.maSV << "#" << sv.hoTen << "#" << sv.soDT << "#" << sv.email << endl;
  }
  file.close();
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

  LoadDuLieuSV();

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


    cout << "_______________________Stages " << i << "_______________________"<< endl;
    cout << "Client connected!" << endl;
    char recvbuf[1024];
    int recvbuflen = 1024;
    iResult = recv(client, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
      cout << "Received: " << recvbuf << endl;

      istringstream iss(recvbuf);
      string token;
      SinhVien sv;
      getline(iss, token, '#');
      sv.maSV = token;
      getline(iss, token, '#');
      sv.hoTen = token;
      getline(iss, token, '#');
      sv.soDT = token;
      getline(iss, token, '#');
      sv.email = token;

      bool found = false;
      for (auto &entry : danhSachSV)
      {
        if (entry.maSV == sv.maSV)
        {
          entry = sv;
          found = true;
          break;
        }
      }

      if (!found)
      {
        danhSachSV.push_back(sv);
      }

      LuuDuLieuSV();

      string response = "Thong tin da duoc cap nhat thanh cong!\0";
      cout << response << endl;
      iResult = send(client, response.c_str(), response.length(), 0);
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