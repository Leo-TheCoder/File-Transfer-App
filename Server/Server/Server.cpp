#include "Server.h"

ListUser users;								//Danh sách users 
vector<SOCKET> clients;						//Danh sách socket client đang kết nối
vector<sockaddr_in> clients_sin;			//Danh sách các địa chỉ client tương ứng
vector<string> username;					//Danh sách tên tài khoản client đang hoạt động tương ứng 
vector<string> logServer;					//Danh sách các log đã được xuất trên server
vector<string> fileList;

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
	//cout << sendStr;								//Xuất log trên server
	logServer.push_back(sendStr);
	RedrawScreen();

	/*
	for (int i = 0; i < clients.size(); i++)		//Xuất log đến các client khác
	{
		bytes = send(clients[i], sendStr.c_str(), sendStr.length() + 1, 0);
	}
	*/

	clients.push_back(client);						//Thêm client này vào danh sách hoạt động
	clients_sin.push_back(sin);

	while (1)	//Vòng lặp nhận các tin nhắn đến từ client
	{
		ZeroMemory(buffer, sizeof(buffer));
		int byte = recv(client, buffer, sizeof(buffer), 0);
		if (byte > 0)
		{
			if (strcmp(buffer, "/quit") == 0)	//Nếu client nhập quit thì thoát khỏi vòng lặp để ngắt kết nối
			{
				break;
			}
			//cout << "From #" << usernameStr << ": " << buffer << endl;	//Xuất ra tin nhắn từ client
			//strcpy(buffer, "");

			if (strcmp(buffer, "refreshlog") == 0)	//Nếu client nhập refreshlog thì gửi lại log
			{
				for (int i = 0; i < logServer.size(); i++) {
					string toSend = logServer[i];
					int iResult = send(client, toSend.c_str(), toSend.length() + 1, 0);
					cout << " ";
				}
				string toSend = "endlog";
				int iResult = send(client, toSend.c_str(), toSend.length() + 1, 0);
				RedrawScreen();
			}

			if (strcmp(buffer, "refreshfilelist") == 0)	//Nếu client nhập refreshfilelist thì gửi lại file list
			{
				for (int i = 0; i < fileList.size(); i++) {
					string toSend = fileList[i];
					int iResult = send(client, toSend.c_str(), toSend.length() + 1, 0);
					cout << " ";
				}
				string toSend = "endlist";
				int iResult = send(client, toSend.c_str(), toSend.length() + 1, 0);
				RedrawScreen();
			}

			if (strcmp(buffer, "upload") == 0)	//Nếu client nhập upload thì nhận upload
			{
				int iResult = FileRecv(client);
				if (iResult == 1) {
					string logStr = "Client #" + usernameStr + " uploaded a file\n";
					//cout << logStr;
					logServer.push_back(logStr);
					RedrawScreen();
				}
			}

			if (strcmp(buffer, "download") == 0)	//Nếu client nhập download thì nhận tên file rồi download
			{
				char* buf = new char[50];
				int iResult = recv(client, buf, 50, 0);

				string filename(buf);

				iResult = FileSend(client, filename);
				if (iResult == 1) {
					string logStr = "Client #" + usernameStr + " downloaded a file\n";
					//cout << logStr;
					logServer.push_back(logStr);
					RedrawScreen();
				}
			/*	else if (iResult == -1) {
					string toSend = "-1";
					send(client, toSend.c_str(), toSend.size() + 1, 0);
					cout << " ";
				}*/
			}
		
		}
		else	//Nếu gửi tin bị lỗi thì ngắt kết nối
		{
			break;
		}
	}
	
	sendStr = "Client #" + usernameStr + " has disconnected\n";	//Xuất log ra server
	//cout << sendStr;
	logServer.push_back(sendStr);

	users.setUserStatusByName(usernameStr, false);

	for (int i = 0; i < username.size(); i++) {
		string tmp = username[i];
		if (usernameStr == tmp) {
			if (username.size() == 1) {
				username.pop_back();
				break;
			}
			else {
				username.erase(username.begin() + i);
				break;
			}
		}
	}
	RedrawScreen();

	//Xuất log ra các client còn lại
	//Thông báo việc đăng xuất của client này
	/*for (int i = 0; i < clients.size(); i++)					
	{							
		//Với client hiện tại thì loại bỏ nó khỏi các danh sách hoạt động 
		if (client == clients[i])
		{
			closesocket(client);
			clients.erase(clients.begin(), clients.begin() + i);
			clients_sin.erase(clients_sin.begin(), clients_sin.begin() + i);
			username.erase(username.begin(), username.begin() + i);
			RedrawScreen();
			break;
		}
	
	}
	RedrawScreen();*/

	/*for (int i = 0; i < clients.size(); i++)
	{
		//Gửi thông báo đến client khác
		bytes = send(clients[i], sendStr.c_str(), sendStr.length() + 1, 0);
	}*/
	
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
	//printf("Bind()\n");
	iResult = bind(socket_descriptor, (const sockaddr*)& sin, sizeof(sin));

	if (iResult == SOCKET_ERROR)
	{
		printf("Can't bind the server's socket!\n");
		return 1;
	}

	//Listen for the connection from clients
	//printf("Listen()\n");
	iResult = listen(socket_descriptor, 5);

	if (iResult == SOCKET_ERROR)
	{
		printf("Can't listen to clients!\n");
		return 1;
	}
	
	int iReadFile = GetFileList("filelist.txt", fileList);

	RedrawScreen();

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
		RedrawScreen();
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


int FileSend(SOCKET socket, string filename)
{
	char tempBuf[40] = "";
	ifstream file;
	file.open(filename, ios::binary);
	if (!file)
	{
		//cout << "Cannot open file" << endl;
		file.close();
		string toSend = "-1";
		send(socket, toSend.c_str(), toSend.size() + 1, 0);
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
	if (recv(socket, filename, 50, 0) == SOCKET_ERROR)
		return -1;
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

	string nameInString(filename);
	fileList.push_back(filename);
	InsertStrToFile(filename, "filelist.txt");
	delete[] filename;
	return 1;
}

void RedrawScreen() {
	system("cls");

	//in list user online
	cout << "Users online:\n";
	for (int i = 0; i < username.size(); i++) {
		cout << " - " << username[i] << endl;
	}

	cout << endl;

	//in list file
	cout << "List of files:\n";
	for (int i = 0; i < fileList.size(); i++) {
		cout << " - " << fileList[i] << endl;
	}

	cout << endl;

	//in log
	cout << "Users' log:\n";
	for (int i = 0; i < logServer.size(); i++) {
		cout << " - " << logServer[i] << endl;
	}
}

int GetFileList(string filename, vector <string>& output)
{
	output.clear();
	ifstream file(filename);
	if (!file.is_open())
	{
		cout << "Can't read file list. " << endl;
		return -1;
	}
	string name;
	while (getline(file, name))
	{
		output.push_back(name);
	}
	file.close();
	return output.size();
}

void InsertStrToFile(string str, string filename)
{
	ofstream file(filename, ios::app);
	if (!file.is_open())
		return;
	file << str << endl;
	file.close();
}