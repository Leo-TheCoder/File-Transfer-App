#pragma once
#include <string>
using namespace std;

class CUser
{
private:
	string username;
	string password;
public:
	string GetUsername();
	string GetPassword();
	void SetUsername(string);
	void SetPassword(string);
};

