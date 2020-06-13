#include "CUser.h"

string CUser::GetUsername()
{
	return username;
}

string CUser::GetPassword()
{
	return password;
}

void CUser::SetUsername(string u)
{
	username = u;
}

void CUser::SetPassword(string p)
{
	password = p;
}
