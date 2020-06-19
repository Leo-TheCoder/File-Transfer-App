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

int FileSend(SOCKET socket, string filename)
{
	char tempBuf[40] = "";
	ifstream file;
	file.open(filename, ios::binary);
	if (!file)
	{
		//cout << "Cannot open file" << endl;
		file.close();
		return -1;
	}
	file.seekg(0, ios::end);
	int fileSize = file.tellg();
	if (fileSize > FILE_LIMIT)
	{
		//cout << "Exceed file limit. File limit is 200MB." << endl;
		return 2;
	}
	file.seekg(0, ios::beg);

	/// Send filename
	if (send(socket, filename.c_str(), filename.size() + 1, 0) == SOCKET_ERROR)
		return -1;
	recv(socket, tempBuf, 40, 0);
	///

	/// Send initial file size
	char fSize[10];
	_itoa_s(int(fileSize), fSize, 10);
	if (send(socket, fSize, 10, 0) == SOCKET_ERROR)
		return -1;
	recv(socket, tempBuf, 40, 0);
	/// 

	char* buffer = new char[fileSize];
	file.read(buffer, fileSize); //read fileSize characters from file
	if (file)
	{
		//cout << "All contents of file read successfully!" << endl;
	}
	else
	{
		int size = file.gcount();
		//cout << "Only " << size << " bytes read" << endl;
		return -1;
	}
	file.close();


	//sending file
	unsigned int bytesSent = 0;
	int bytesToSend = 0;
	while (bytesSent < fileSize)
	{
		//if data to be sent is greater than buffer size to be send
		if (fileSize - bytesSent >= BYTE_CHUNK)
			bytesToSend = BYTE_CHUNK;
		else
			bytesToSend = fileSize - bytesSent;

		//send chunks of files
		int iResult = send(socket, buffer + bytesSent, bytesToSend, 0);
		if (iResult != SOCKET_ERROR)
		{
			if (iResult > 0)
				bytesSent += iResult;
			if (iResult == 0) {
				//cout << "Connection closed\n";
				return -1;
			}
		}
		else
		{
			//cout << "Sending error #" << WSAGetLastError() << endl;
			return -1;
		}
		if (recv(socket, tempBuf, 40, 0) == SOCKET_ERROR)
		{
			//cout << "Connection disrupt! Error: " << WSAGetLastError() << endl;
			return -1;
		}
	}
	delete[] buffer;
	return 1;
}

int FileRecv(SOCKET socket)
{
	/// receive filename
	char* filename = new char[50];
	if (recv(socket, filename, 50, 0) == SOCKET_ERROR) {
		return -1;
	}
	else if (strcmp(filename, "-1") == 0) {
		return -1;
	}
	if (send(socket, "OK", strlen("OK"), 0) == SOCKET_ERROR)
		return -1;
	///



	/// receive initial file size
	char fSize[10];
	if (recv(socket, fSize, 10, 0) == SOCKET_ERROR)
		return -1;
	if (send(socket, "OK", strlen("OK"), 0) == SOCKET_ERROR)
		return -1;
	int fileSize = atoi(fSize);
	/// 

	int bytesRecv = 0;
	int bytesToRecv = 0;
	char* buffer = new char[BYTE_CHUNK];

	ofstream outFile(filename, ios::binary);
	while (bytesRecv < fileSize)
	{
		//if data recv is greater than buffer size to be send
		if (fileSize - bytesRecv >= BYTE_CHUNK)
			bytesToRecv = BYTE_CHUNK;
		else
			bytesToRecv = fileSize - bytesRecv;

		//recv chunks of files
		int iResult = recv(socket, buffer, bytesToRecv, 0);
		if (iResult != SOCKET_ERROR)
		{
			if (iResult > 0)
			{
				//write file
				if (bytesToRecv != BYTE_CHUNK)
					buffer[bytesToRecv] = '\0';
				bytesRecv += iResult;
				outFile.write(buffer, bytesToRecv);
			}
			if (iResult == 0) {
				//cout << "Connection closed\n";
				delete[] buffer;
				delete[] filename;
				outFile.close();
				return -1;
			}
		}
		else
		{
			//cout << "Receving error #" << WSAGetLastError() << endl;
			return -1;
		}
		if (send(socket, "OK", strlen("OK"), 0) == SOCKET_ERROR)
			return -1;
	}
	delete[] buffer;
	outFile.close();
	delete[] filename;
	return 1;
}
