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

ListUser users;								//Danh sách users 
vector<SOCKET> clients;						//Danh sách socket client đang kết nối
vector<sockaddr_in> clients_sin;			//Danh sách các địa chỉ client tương ứng
vector<string> username;					//Danh sách tên tài khoản client đang hoạt động tương ứng 

//Kiểm tra login thành công hay không
//Ret 1: thành công
//Ret 0: lỗi hoặc nhập sai tên hay mật khẩu
//Ret 2: account đang được sử dụng
//Trả về username sẽ là tên tk của client đó
int checkLogIn(SOCKET client, ListUser& list, string& username);

//Thread để server tương tác với client
DWORD WINAPI threadClient(LPVOID socket)
{
	CParam* myClient = (CParam*) socket;			//Chuyển về dạng CParam*
	SOCKET client = myClient->getSocket();
	sockaddr_in sin = myClient->getSin();

	string usernameStr;
	
	int res = checkLogIn(client, users, usernameStr);		//Kiểm tra kết quả đăng nhập
	if (res == 0 || res == 2)						//Nếu đăng nhập lỗi
	{												//Hoặc đăng nhập sai
		closesocket(client);						//Hoặc đăng nhập vào tk đang được sử dụng
		ExitThread(0);								//Thì thoát khỏi thread
	}

	string sendStr;
	username.push_back(usernameStr);				//Lưu tên tk vào danh sách hoạt động

	
	sendStr = "Client #" + usernameStr + " has connected\n";
	cout << sendStr;								//Xuất log trên server

	for (int i = 0; i < clients.size(); i++)		//Xuất log đến các client khác
	{
		int bytes = send(clients[i], sendStr.c_str(), sendStr.length() + 1, 0);
	}

	clients.push_back(client);						//Thêm client này vào danh sách hoạt động
	clients_sin.push_back(sin);
	

	char buffer[2048];

	while (1)	//Vòng lặp nhận các tin nhắn đến từ client
	{
		int byte = recv(client, buffer, sizeof(buffer), 0);
		if (byte > 0)
		{
			if (strcmp(buffer, "/quit") == 0)	//Nếu client nhập quit thì thoát khỏi vòng lặp để ngắt kết nối
			{
				break;
			}
			printf("From #%d: %s\n", client, buffer);	//Xuất ra tin nhắn từ client
			strcpy(buffer, "");

			//.................
			//...................
			//Thêm xử lí nhận upload ở đây (I think :v)
		}
		else	//Nếu gửi tin bị lỗi thì ngắt kết nối
		{
			break;
		}
		//---------------------------------------------
		//--- Đây là chỗ để xử lí việc nhận tín hiệu
		//--- download hay upload từ client
		//--- Cần thêm 1 thread để xử lí download từ đây
		//---------------------------------------------

	}
	
	sendStr = "Client #" + usernameStr + " has logged out\n";	//Xuất log ra server
	cout << sendStr;

	users.getUserByName(usernameStr)->setStatus(false);			//Điều chỉnh trạng thái đặng nhập lại cho user

	//Xuất log ra các client còn lại
	//Thông báo việc đăng xuất của client này
	for (int i = 0; i < clients.size(); i++)					
	{							
		//Với client hiện tại thì loại bỏ nó khỏi các danh sách hoạt động 
		if (client == clients[i])
		{
			closesocket(client);
			clients.erase(clients.begin(), clients.begin() + i);
			clients_sin.erase(clients_sin.begin(), clients_sin.begin() + i);
			username.erase(username.begin(), username.begin() + i);
			i--;
			continue;
		}
		//Gửi thông báo đến client khác
		int bytes = send(clients[i], sendStr.c_str(), sendStr.length() + 1, 0);
	}
	
	//Thoát thread
	ExitThread(0);
}

int main()
{

	users.updateList();								//Lấy từ file tất cả thông tin các account --> users
	
	WORD wVersionRequested;
	WSADATA wsaData;

	DWORD thread;	//Kết quả trả về của thread

	SOCKET socket_descriptor = INVALID_SOCKET;		//Socket dùng để lắng nghe từ client
	struct sockaddr_in sin;							//Địa chỉ của server


	int iResult;
	// Initialize Winsock
	wVersionRequested = MAKEWORD(2, 2);
	iResult = WSAStartup(wVersionRequested, &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	memset(&sin, 0, sizeof(sin));		//Initialize sockaddr
	sin.sin_family = AF_INET;			//Server IP's type = IPv4
	sin.sin_addr.s_addr = INADDR_ANY;	//Can connect to any interface
	sin.sin_port = htons(5800);			//The port number is 5800

	//Create the server's socket
	socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_descriptor == INVALID_SOCKET)
	{
		printf("Can't create the server's socket!\n");
		return 1;
	}

	//Bind the socket to the port (port 5800)
	printf("Bind()\n");
	iResult = bind(socket_descriptor, (const sockaddr*)& sin, sizeof(sin));

	if (iResult == SOCKET_ERROR)
	{
		printf("Can't bind the server's socket!\n");
		return 1;
	}

	//Listen for the connection from clients
	printf("Listen()\n");
	iResult = listen(socket_descriptor, 5);

	if (iResult == SOCKET_ERROR)
	{
		printf("Can't listen to clients!\n");
		return 1;
	}
	
	SOCKET tmpClient;							
	sockaddr_in tmpClient_sin;					
	int len_sin = sizeof(tmpClient_sin);		

	CParam* myClient = new CParam;			//Struct dùng để truyền nhiều tham số vào thread
	
	while (1)
	{
		//Accept the connection from client
		tmpClient = accept(socket_descriptor, (sockaddr*)& tmpClient_sin, &len_sin);

		myClient->setSocket(tmpClient);
		myClient->setSin(tmpClient_sin);
		
		//Thread dùng để giải quyết riêng với client
		CreateThread(NULL, 0, threadClient, (LPVOID)myClient, 0, &thread);
		Sleep(50);

		//------------------------WARNING------------------------------
		//---	1) Có 1 bug làm cho chương trình bị văng --------------
		//---		khi các client đều đã ngắt kết nối     ------------
		//-------------------------------------------------------------
		//---	2) Chưa tạo thread để xử lí riêng dành cho server -----
		//---		ví dụ như thread để nhận lệnh từ server   ---------
		//---			để thoát khỏi vòng lặp     --------------------
		//-------------------------------------------------------------

	}

	//Close socket
	closesocket(socket_descriptor);
	WSACleanup();
	return 0;
}

int checkLogIn(SOCKET client, ListUser& list, string& username)
{
	char buff[1024];
	//Nhận thông tin từ client về tên tk và mật khẩu
	//Có dạng buff = "username,password"
	int byte = recv(client, buff, sizeof(buff), 0);
	if (byte == 0 || byte == SOCKET_ERROR)
	{
		return 0;
	}
	User guess;
	string name = "";
	string pass = "";

	//Tách buff ra 2 thành phần

	int i = 0;
	for (i = 0; i < strlen(buff); i++)
	{
		name += buff[i];
	}
	i = name.find(',');
	pass = name.substr(i + 1);	//Biến chứa mật khẩu
	name = name.substr(0, i);	//Biến chứa tên tk

	guess.setUsername(name);
	username = name;		//Lưu tên tk lại
	guess.setPassword(pass);

	//Kiểm tra xem thông tin đã khớp hay chưa
	int res = list.checkList(guess);	
	if (res == 1)	//Đăng nhập thành công
	{
		strcpy(buff, "1");
		byte = send(client, buff, sizeof(buff), 0);
		return 1;
	}
	else if(res == 0)	//Đăng nhập thất bại (nhập sai)
	{
		strcpy(buff, "0");
		byte = send(client, buff, sizeof(buff), 0);
		return 0;
	}
	else				//Tk đang được sử dụng
	{
		strcpy(buff, "2");
		byte = send(client, buff, sizeof(buff), 0);
		return 2;
	}
}