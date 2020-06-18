#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>
using namespace std;
#pragma comment (lib, "ws2_32.lib")
int FileSend(SOCKET socket, string filename);
int FileRecv(SOCKET socket);
int SendData(SOCKET s,const char* data);
int ReceiveData(SOCKET s, char* data, int size);
