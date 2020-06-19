#pragma once
#include <WinSock2.h>

class CParam
{
private:
	SOCKET mySocket;
	sockaddr_in mySin;
public:
	void setSocket(SOCKET newSocket);
	void setSin(sockaddr_in newSin);
	SOCKET getSocket();
	sockaddr_in getSin();
};

