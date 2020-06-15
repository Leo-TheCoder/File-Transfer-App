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
using namespace std;
#pragma comment (lib, "Ws2_32.lib")

//Thread xử lí các dữ liệu được gửi từ server
DWORD WINAPI recvFromServer(LPVOID socket);

//Hàm gửi xử lí quá trình đăng nhập
//Ret 0: lỗi hoặc đăng nhập sai
//Ret 1: thành công
//Ret 2: Account đang được sử dụng
int logIn(SOCKET server);

int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int retcode;

	SOCKET socket_descriptor;		//Socket của client
	struct sockaddr_in sin;			//Địa chỉ của client

	char Buffer[4096];
	int length;


	/*
	[1] WSAStartup() must be called before any other socket
	routines.  The following code prints all returned
	information valid in WinSock 2.

	WSAStartup returns the following information in the
	WSAData structure.

	struct WSAData {
	WORD            wVersion;
	WORD            wHighVersion;
	char            szDescription[WSADESCRIPTION_LEN+1];
	char            szSystemStatus[WSASYSSTATUS_LEN+1];
	unsigned short  iMaxSockets; (ignore in WinSock 2)
	unsigned short  iMaxUdpDg;   (ignore in WinSock 2)
	char FAR *      lpVendorInfo;(ignore in WinSock 2)
	};

	Note that WSAGetLastError() should not be used since the
	error code may not be saved properly if winsock.dll did
	not load.
	*/

	wVersionRequested = MAKEWORD(2, 2);	// Use MAKEWORD(1,1) if you're at WinSock 1.1
	retcode = WSAStartup(wVersionRequested, &wsaData);
	if (retcode != 0)
	{
		printf("Startup failed: %d\n", retcode);
		return 1;
	}


	/*
	[2] Once WSAStartup has been called, the socket can be
	created using the socket() call.  The following creates an
	Internet protocol family (PF_INET) socket providing stream
	service (SOCK_STREAM).
	*/

	printf("socket()\n");
	socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_descriptor == INVALID_SOCKET)
	{
		printf("Can't create the client's socket!\n");
		return 1;
	}


	/*
	[3] Before making a connection, an Internet address
	family structure must be initialized.

	struct sockaddr_in {
	short sin_family;
	u_short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
	};

	It is first set to all zeros.  The family is AF_INET.
	We'll use 4984 as the port number for this example.
	htons() converts to network byte ordering.

	gethostbyname() is used to determine the address of the
	remote host.  For this example, we connect to the host
	specified in the first command line argument or the local
	host ("localhost") if no command line arguments are
	provided, using the first address for this host in the
	h_addr_list.

	gethostbyname() returns a pointer to the following
	structure.  Some of the contents are printed below.

	struct hostent {
	char FAR * h_name;
	char FAR * FAR * h_aliases;
	short h_addrtype;
	short h_length;
	char FAR * FAR * h_addr_list;
	};
	*/

	memset(&sin, 0, sizeof(sin));	//Initialize sockadrr
	sin.sin_family = AF_INET;		//Client IP's type = IPv4
	sin.sin_port = htons(5800);		//The port number is 5800

	string id;
	printf("Get server's IP address: ");	//Nhập IP server
	getline(cin, id);

	sin.sin_addr.s_addr = inet_addr(id.c_str());	//Truyền IP đó vào sin
	//------------WARNING------------------
	//--- Thêm xử lí việc nhập tào lao-----
	//--- Dẫn đến client bị đứng ----------
	//-------------------------------------


	/*
	[4] connect() is used to establish a connection to a remote
	endpoint.
	*/

	printf("connect()\n");

	retcode = connect(socket_descriptor, (struct sockaddr*) & sin, sizeof(sin));
	if (retcode == SOCKET_ERROR)
	{
		printf("Can't connect to the server!\n");
		return 1;
	}

	DWORD thread;
	int res = logIn(socket_descriptor);				//Xử lí đăng nhập
	if (res != 1)	//Nếu đăng nhập thất bại thì tắt client
	{
		_getch();
		closesocket(socket_descriptor);
		return 1;
	}
	
	//Thread xử lí nhận dữ liệu từ server (log, download)
	CreateThread(NULL, 0, recvFromServer, (LPVOID)socket_descriptor, 0, &thread);
	while (1)
	{
		//Main thread xử lí gửi dữ liệu (command, upload)
		string sendStr;
		getline(cin, sendStr);
		int bytes = send(socket_descriptor, sendStr.c_str(), sendStr.length() + 1, 0);

		//Nếu gửi thất bại (kết nối thất bại) hay có lệnh hủy ("/quit") thì tắt client
		if (bytes == 0 || bytes == SOCKET_ERROR || sendStr == "/quit")
		{
			break;
		}

		//...............
		//Thêm xử lí upload ở đây nè (I think :v)

	}

	/*
	[8] The client closes its socket using closesocket();
	*/

	printf("closesocket()\n");

	retcode = closesocket(socket_descriptor);
	if (retcode == SOCKET_ERROR)
	{
		printf("Close socket failed: %d\n", WSAGetLastError());
		return 1;
	}

	/*
	[9] WSACleanup() is used to terminate use of socket services.
	*/

	printf("WSACleanup()\n");

	retcode = WSACleanup();
	if (retcode == SOCKET_ERROR)
	{
		printf("Cleanup failed: %d\n", WSAGetLastError());
	}

	_getch();
	return 0;
}

//Thread nhận dữ liệu từ server
DWORD WINAPI recvFromServer(LPVOID socket)
{
	SOCKET server = (SOCKET)socket;
	char buff[2048];
	while (1)	//Hiện tại chỉ đơn giản là gửi gì thì ghi ra đó (ghi log)
	{
		int bytes = recv(server, buff, sizeof(buff), 0);
		if (bytes > 0)
		{
			printf("%s", buff);
			strcpy(buff, "");
		}
		//.................
		//.................
		//Xử lí nhận download ở đây (I guess :v)
	}
	ExitThread(0);
}

//Hàm xử lí đăng nhập
int logIn(SOCKET server)
{
	
	User guess;					//---------------------------
	string temp;				//---------------------------
	cout << "Username: ";		//---------------------------
	getline(cin, temp);			//------Nhập thông tin ------
	guess.setUsername(temp);	//---------------------------
	cout << "Password: ";		//---------------------------
	getline(cin, temp);			//---------------------------
	guess.setPassword(temp);	//---------------------------
	cin.clear();
	

	//Ghép username và password là theo dạng
	//Str = "username,password"
	temp = guess.getUsername() + "," + guess.getPassword();
	int byte = send(server, temp.c_str(), temp.length() + 1, 0);
	if (byte == 0 || byte == SOCKET_ERROR)
	{
		return -1;
	}
	char buff[1024];
	//Nhận lại tín hiệu từ server để xem thông tin đăng nhập thế nào
	byte = recv(server, buff, sizeof(buff), 0);
	if (byte > 0)
	{
		if (strcmp(buff, "0") == 0)
		{
			cout << "Wrong username or password!\n";
			return 0;
		}
		else if (strcmp(buff, "1") == 0)
		{
			cout << "Login sucessfully!\n";
			return 1;
		}
		else
		{
			cout << "This account has already been used! Try again later!\n";
			return 2;
		}
	}
}