#define _CRT_SECURE_NO_WARNINGS
#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "ServerModules.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#include <iostream>
#include <string>
#include <vector>

#include "CUser.h"

using namespace std;

int AcceptLogIn(SOCKET, vector <CUser>);

int CheckUserForLoggingIn(char*, char*, vector <CUser>);
void ReadFileForUserDatabase(vector <CUser>&);