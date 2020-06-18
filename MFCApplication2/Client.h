#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
//#include <WinSock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#include <vector>
#include "User.h"
using namespace std;
#pragma comment (lib, "Ws2_32.lib")

//Thread xử lí các dữ liệu được gửi từ server
DWORD WINAPI recvFromServer(LPVOID socket, CListCtrl);

//Hàm gửi xử lí quá trình đăng nhập
//Ret 0: lỗi hoặc đăng nhập sai
//Ret 1: thành công
//Ret 2: Account đang được sử dụng
int LogIn(SOCKET server, string, string);
