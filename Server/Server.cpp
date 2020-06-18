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
vector<string> logServer;					//Danh sách các log đã được xuất trên server

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

//Thread để server tương tác với client
DWORD WINAPI threadClient(LPVOID socket)
{
	CParam* myClient = (CParam*) socket;			//Chuyển về dạng CParam*
	SOCKET client = myClient->getSocket();
	sockaddr_in sin = myClient->getSin();

	string usernameStr;
	char buffer[1024];

	//Nhận xem client muốn đăng nhập hay đăng kí
	int bytes = recv(client, buffer, sizeof(buffer), 0);
	if (bytes == SOCKET_ERROR || bytes <= 0)
	{
		closesocket(client);
		return 0;
	}
	if (strcmp(buffer, "0") == 0)	//Client muốn đăng kí
	{
		int res = checkSignUp(client, users, usernameStr);
		if (res < 0)
		{
			closesocket(client);
			return 0;
		}
		
	}
	else	//Client muốn đăng nhập
	{
		int res = checkLogIn(client, users, usernameStr);		//Kiểm tra kết quả đăng nhập
		if (res == 0 || res == 2)						//Nếu đăng nhập lỗi
		{												//Hoặc đăng nhập sai
			closesocket(client);						//Hoặc đăng nhập vào tk đang được sử dụng
			return 0;									//Thì thoát khỏi thread
		}
	}
	

	string sendStr;
	username.push_back(usernameStr);				//Lưu tên tk vào danh sách hoạt động

	
	sendStr = "Client #" + usernameStr + " has connected\n";
	cout << sendStr;								//Xuất log trên server
	logServer.push_back(sendStr);

	for (int i = 0; i < clients.size(); i++)		//Xuất log đến các client khác
	{
		bytes = send(clients[i], sendStr.c_str(), sendStr.length() + 1, 0);
	}

	clients.push_back(client);						//Thêm client này vào danh sách hoạt động
	clients_sin.push_back(sin);
	



	while (1)	//Vòng lặp nhận các tin nhắn đến từ client
	{
		int byte = recv(client, buffer, sizeof(buffer), 0);
		if (byte > 0)
		{
			if (strcmp(buffer, "/quit") == 0)	//Nếu client nhập quit thì thoát khỏi vòng lặp để ngắt kết nối
			{
				break;
			}
			cout << "From #" << usernameStr << ": " << buffer << endl;	//Xuất ra tin nhắn từ client
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
	logServer.push_back(sendStr);

	users.setUserStatusByName(usernameStr, false);

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
			break;
		}
		
	}

	for (int i = 0; i < clients.size(); i++)
	{
		//Gửi thông báo đến client khác
		bytes = send(clients[i], sendStr.c_str(), sendStr.length() + 1, 0);
	}
	
	//Thoát thread
	return 0;
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
	HANDLE clientThread;

	while (1)
	{
		//Accept the connection from client
		tmpClient = accept(socket_descriptor, (sockaddr*)& tmpClient_sin, &len_sin);

		myClient->setSocket(tmpClient);
		myClient->setSin(tmpClient_sin);
		
		//Thread dùng để giải quyết riêng với client
		clientThread = CreateThread(NULL, 0, threadClient, (LPVOID)myClient, 0, &thread);
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


int checkSignUp(SOCKET client, ListUser& list, string& username)
{
	char buffer[1024];
	int bytes = 0;
	string name = "";
	string password = "";
	while (1)			//Vòng lặp kiểm tra username nhập vào từ client cho đến khi hợp lệ
	{
		//Nhận username
		bytes = recv(client, buffer, sizeof(buffer), 0);
		if (bytes <= 0)
		{
			return -1;
		}
		
		name = buffer;	
		if (!list.existUsername(name)) {	//Kiểm tra xem username tồn tại hay chưa
			bytes = send(client, "1", sizeof("1"), 0);	//Gửi thông báo thành công
			if (bytes <= 0)
			{
				return -1;
			}
			break;
		}
		bytes = send(client, "0", sizeof("0"), 0);	//Gửi thông báo thất bại
		if (bytes <= 0)
		{
			return -1;
		}
	}

	bytes = recv(client, buffer, sizeof(buffer), 0);	//Nhận password
	if (bytes <= 0)
	{
		return -1;
	}
	password = buffer;

	User newUser;
	newUser.setUsername(name);
	username = name;
	newUser.setPassword(password);
	newUser.setStatus(true);	//Thiết lập trạng thái hoạt động
	list.addNewUser(newUser);
	bytes = send(client, "1", sizeof("1"), 0);	//Xác nhận lại với client rằng đăng nhập thành công
	if (bytes <= 0)
	{
		return -1;
	}
	return 1;

}