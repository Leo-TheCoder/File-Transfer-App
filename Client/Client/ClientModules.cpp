#include "ClientModules.h"


/*
Hàm được gọi để client có thể log in
	- Input: socket dùng để gửi thông tin
	- Output: 1 nếu không log in được
			  2 nếu log in thành công
Nội dung:
	- Cho client nhập username và password
	- Gửi cho server để check với database
	- Server gửi lại kết quả (0 là đúng, 1 là sai)
	- In ra màn hình kết quả
*/
int LogIn(SOCKET sock)
{
	string username, password;
	int sendResult; //biến giữ giá trị trả về của hàm send
	char* receivedContent = new char[4096];
	int bytesReceived;

	cout << "Username: ";
	cin >> username;
	cout << "Password: ";
	cin >> password;

	if ((username.length() > 0) && (password.length() > 0))		// Make sure the user has typed in something
	{
		// Send the text
		sendResult = send(sock, username.c_str(), username.size() + 1, 0);
		cout << " ";
		if (sendResult != SOCKET_ERROR)
		{
			sendResult = send(sock, password.c_str(), password.size() + 1, 0);
			if (sendResult != SOCKET_ERROR) {
				// Wait for response
				ZeroMemory(receivedContent, 4096);
				bytesReceived = recv(sock, receivedContent, 4096, 0);
				string recv(receivedContent);
				if (bytesReceived > 0)
				{
					if (recv == "1") {
						cout << "Wrong username or password!\n";
						return 1;
					}
					else {
						cout << "Logged in successfully!\n";
						return 0;
					}
				}
				else {
					return 1;
				}
			}
		}
	}

	return 0;
}

int Register(SOCKET sock)
{
	string username, password;
	int sendResult; //biến giữ giá trị trả về của hàm send
	char* receivedContent = new char[4096];
	int bytesReceived;

	cout << "Enter username and password to register.\n";
	cout << "Username: ";
	cin >> username;
	cout << "Password: ";
	cin >> password;

	if ((username.length() > 0) && (password.length() > 0))		// Make sure the user has typed in something
	{
		// Send the text
		sendResult = send(sock, username.c_str(), username.size() + 1, 0);
		cout << " ";
		if (sendResult != SOCKET_ERROR)
		{
			sendResult = send(sock, password.c_str(), password.size() + 1, 0);
			if (sendResult != SOCKET_ERROR) {
				// Wait for response
				ZeroMemory(receivedContent, 4096);
				bytesReceived = recv(sock, receivedContent, 4096, 0);
				string recv(receivedContent);
				if (bytesReceived > 0)
				{
					if (recv == "1") {
						cout << "Username not available!\n";
						return 1;
					}
					else {
						cout << "Registered successfully!\n";
						return 0;
					}
				}
				else {
					return 1;
				}
			}
		}
	}

	return 0;
}
