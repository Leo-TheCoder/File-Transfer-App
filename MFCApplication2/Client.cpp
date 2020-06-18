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

//XỬ lý đăng ký
int Register(SOCKET server, string username, string password)
{
	User guess;
	string temp;
	char buffer[1024];
	int bytes = 0;
	do			//Vòng lặp kiểm tra username cho đến khi nào hợp lệ
	{
		temp = username;
		cout << "\n";
		//Gửi username cho server check
		bytes = send(server, temp.c_str(), temp.length() + 1, 0);
		if (bytes == SOCKET_ERROR || bytes <= 0)
		{
			//cout << "Sending fail!!\n";
			return -1;
		}

		//Nhận thông báo thành công hay thất bại từ server
		bytes = recv(server, buffer, sizeof(buffer), 0);
		if (bytes == SOCKET_ERROR || bytes <= 0)
		{
			//cout << "Receiving fail!!\n";
			return -1;
		}
		else if (strcmp(buffer, "1") == 0)	//Thành công
		{
			break;
		}
		else {
			//Nếu thất bại thì xuất dòng này
		//cout << "This username has already existed! Try again\n";
			return 0;
		}

		//Sau đó lặp lại quá trình
	} while (1);

	guess.setUsername(temp);
	//cout << "Password: ";
	//getline(cin, temp);
	temp = password;
	guess.setPassword(temp);
	cout << "\n";
	//Gửi password để server lưu lại
	bytes = send(server, temp.c_str(), temp.length() + 1, 0);
	if (bytes <= 0)
	{
		return -1;
	}

	//Nhận thông báo thành công
	bytes = recv(server, buffer, sizeof(buffer), 0);
	if (bytes <= 0)
	{
		return -1;
	}

	//cout << "Login sucessfully!\n";
	return 1;
}