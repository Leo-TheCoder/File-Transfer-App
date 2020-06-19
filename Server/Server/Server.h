#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include "User.h"
#include "CParam.h"

#pragma comment (lib, "Ws2_32.lib")

#include <fstream>
#define BYTE_CHUNK 4096
#define FILE_LIMIT 209715200

//Kiểm tra login thành công hay không
//Ret 1: thành công
//Ret 0: lỗi hoặc nhập sai tên hay mật khẩu
//Ret 2: account đang được sử dụng
//Trả về username sẽ là tên tk của client đó
int checkLogIn(SOCKET client, ListUser& list, string& username);

//Kiểm tra signup thành công hay không
//Ret 1: thành công
//Ret -1: lỗi 
int checkSignUp(SOCKET client, ListUser& list, string& username);

//Hàm xử lí gửi file (upload)
//Ret -1: lỗi
//Ret 1 thành công
//Ret 2: file quá lớn
int FileSend(SOCKET socket, string filename);

//Hàm nhận file (download)
//Ret -1: lỗi
//Ret 1: thành công
int FileRecv(SOCKET socket);

bool CheckStringVector(vector <string> list, string input);