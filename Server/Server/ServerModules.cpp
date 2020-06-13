#include "ServerModules.h"


/*
Hàm nhận thông tin đăng nhập và trả về kết quả
    - Input: Socket client, userList để kiểm tra 
    - Output: 1 khi sai, 0 khi đúng
Nội dung:
    - Nhận lần lượt 2 string username và password từ socket client
    - Check trong database xem có đúng hay không
    - Gửi lại kết quả cho client (1 khi sai, 0 khi đúng)
*/
int AcceptLogIn(SOCKET client, vector <CUser> userList)
{
	char *username = new char[4096], *password = new char[4096];
	int bytesReceived, sendResult;


	//nhận thông tin username
	bytesReceived = recv(client, username, 4096, 0);
	//nếu không nhận được thì trả về 1
	if ((bytesReceived == SOCKET_ERROR) || (bytesReceived == 0))
	{
		return 1;
	}

	
	//nhận thông tin password
	bytesReceived = recv(client, password, 4096, 0);
	//nếu không nhận được thì trả về 1
	if ((bytesReceived == SOCKET_ERROR) || (bytesReceived == 0))
	{
		return 1;
	}
	
	//check trong database
	int checkResult = CheckUserForLoggingIn(username, password, userList);
	if (checkResult == 0) {
		sendResult = send(client, "0", 4096, 0);
	}
	else {
		sendResult = send(client, "1", 4096, 0);
	}

	return 0;
}

/*
Hàm để kiểm tra username và password trong database
	- Input: 2 mảng char* username và password
	- Output: 1 khi sai, 0 khi đúng
Nội dung:
	- Đổi 2 input qua string để dễ xử lý
	- Chạy vòng lặp qua vector chứa CUser để kiểm tra
*/
int CheckUserForLoggingIn(char* username, char* password, vector <CUser> userList)
{
	string u(username), p(password); //chuyển char* về string

	for (int i = 0; i < userList.size(); i++) {
		string tmpU = userList[i].GetUsername(); //lấy username lưu vào tmpU
		if (u == tmpU) { //nếu username đưa vào trùng với username trong database thì xét tiếp password
			string tmpP = userList[i].GetPassword(); //lấy password lưu vào tmpP
			if (p == tmpP) {
				return 0; //đúng password thì trả về 0
			}
			else {
				return 1; //sai trả về 1
			}
		}
	}
	return 1; //không có acc trong database nên trả về 1
}

/*
Hàm đọc danh sách user từ file txt lưu vào vector
	- Input: vector lưu danh sách user
Nội dung:
	- Mở file, đọc theo cú pháp <username password> theo từng dòng
	- Username và password cách nhau 1 dấu cách, 1 user 1 dòng
*/
void ReadFileForUserDatabase(vector <CUser> &userList)
{
	string username, password;
	freopen("UserList.txt", "r", stdin); //mở file UserList.txt để đọc
	while (!cin.eof()) {
		cin >> username;
		cin >> password;
		
		CUser tmp;
		tmp.SetUsername(username);
		tmp.SetPassword(password);
		userList.push_back(tmp);
	}
	fclose(stdin);
	/*freopen("CON", "rt", stdin);
	cin.clear();
	cin.ignore();*/
}
