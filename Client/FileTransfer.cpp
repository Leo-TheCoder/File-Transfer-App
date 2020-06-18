#include "FileTransfer.h"

#include <fstream>
#define BYTE_CHUNK 4096
#define FILE_LIMIT 209715200
using namespace std;
int FileSend(SOCKET socket, string filename)
{	
	char tempBuf[40] = "";
	ifstream file;
	file.open(filename, ios::binary);
	if (!file)
	{
		cout << "Cannot open file" << endl;
		file.close();
		return -1;
	}
	file.seekg(0, ios::end);
	int fileSize = file.tellg();
	if (fileSize > FILE_LIMIT)
	{
		cout << "Exceed file limit. File limit is 200MB." << endl;
		return -1;
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
		cout << "All contents of file read successfully!" << endl;
	}
	else
	{
		int size = file.gcount();
		cout << "Only " << size << " bytes read" << endl;
	}
	file.close();


	//sending file
	unsigned int bytesSent = 0;
	int bytesToSend = 0;
	while (bytesSent < fileSize)
	{
		//if data to be send is greater than buffer size to be send
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
				cout << "Connection closed\n";
				return -1;
			}
		}
		else
		{
			cout << "Sending error #" << WSAGetLastError() << endl;
		}
		if (recv(socket, tempBuf, 40, 0) == SOCKET_ERROR)
		{
			cout << "Connection disrupt! Error: " << WSAGetLastError() << endl;
		}
	}
	delete[] buffer;
	return 1;
}

int FileRecv(SOCKET socket)
{
	/// receive filename
	char *filename = new char[50];
	if (recv(socket,filename,50,0)==SOCKET_ERROR)
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
				cout << "Connection closed\n";
				delete[] buffer;
				delete[] filename;
				outFile.close();
				return -1;
			}
		}
		else
		{
			cout << "Receving error #" << WSAGetLastError() << endl;
		}
		if (send(socket, "OK", strlen("OK"), 0) == SOCKET_ERROR)
			return -1;
	}
	delete[] buffer;
	delete[] filename;
	outFile.close();
	return 1;
}

int SendData(SOCKET s,const char* data)
{
	return send(s, data, strlen(data), 0);
}

int ReceiveData(SOCKET s, char* data, int size)
{
	int result = recv(s, data, size + 1, 0);
	data[size] = '\0';
	return result;
}
