#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "Client.h"
#include "ClientScreenDlg.h"



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
			char* conver = buff;
			LPCTSTR logString = (LPCTSTR)conver;
			//userLog.InsertItem(0, logString);
			//printf("%s", buff);
			strcpy(buff, "");
		}
		//.................
		//.................
		//Xử lí nhận download ở đây (I guess :v)
	}
	ExitThread(0);
}

//Hàm xử lí đăng nhập
int LogIn(SOCKET server, string username, string password)
{

	User guess;					//---------------------------
	string temp;	
	/*//---------------------------
	cout << "Username: ";		//---------------------------
	getline(cin, temp);			//------Nhập thông tin ------
		//---------------------------
	cout << "Password: ";		//---------------------------
	getline(cin, temp);			//---------------------------
		//---------------------------
	cin.clear();*/

	guess.setUsername(username);
	guess.setPassword(password);

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
			//cout << "Wrong username or password!\n";
			return 0;
		}
		else if (strcmp(buff, "1") == 0)
		{
			//cout << "Login sucessfully!\n";
			return 1;
		}
		else
		{
			//cout << "This account has already been used! Try again later!\n";
			return 2;
		}
	}
}