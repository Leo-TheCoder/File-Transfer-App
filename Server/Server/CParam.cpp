#include "CParam.h"

void CParam::setSocket(SOCKET newSocket)
{
	mySocket = newSocket;
}
void CParam::setSin(sockaddr_in newSin)
{
	mySin = newSin;
}
SOCKET CParam::getSocket()
{
	return mySocket;
}
sockaddr_in CParam::getSin()
{
	return mySin;
}