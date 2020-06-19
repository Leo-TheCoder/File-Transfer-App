#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "User.h"

void User::setUsername(string name)
{
	username = name;
}
void User::setPassword(string pass)
{
	password = pass;
}
void User::setStatus(bool stt)
{
	status = stt;
}
string User::getPassword()
{
	return password;
}
string User::getUsername()
{
	return username;
}
bool User::getStatus()
{
	return status;
}

User::User()
{
	username = "";
	password = "";
	status = false;
}


void ListUser::updateList()
{
	string temp;
	string username;
	string password;
	vector<User> newList;
	freopen("accounts.txt", "r", stdin);
	//Đơn giản là đọc file
	//Các tk sẽ có dạng "username,password"
	//Mỗi dòng là mỗi tk
	while (!cin.eof())
	{
		User s;
		getline(cin, temp);
		int i_separate = temp.find(',');
		username = temp.substr(0, i_separate);
		password = temp.substr(i_separate + 1);
		s.setUsername(username);
		s.setPassword(password);
		newList.push_back(s);
	}

	listUsers = newList;

	cin.clear();
	cin.ignore();
	freopen("CON", "rt", stdin);
	cin.clear();
}

bool User::operator==(User a)
{
	if (this->username == a.username &&
		this->password == a.password)
	{
		return true;
	}
	return false;
}

void ListUser::writeAll()
{
	for (int i = 0; i < listUsers.size(); i++)
	{
		cout << "Username: " << listUsers[i].getUsername() << endl;
		cout << "Password: " << listUsers[i].getPassword() << endl;
		cout << endl;
	}
}

//Kiểm tra trạng thái đăng nhập
int ListUser::checkList(User guess)
{
	for (int i = 0; i < listUsers.size(); i++)
	{
		//Tìm ra rồi nè
		if (guess == listUsers[i])
		{
			//Xem có được xài hay chưa
			if (listUsers[i].getStatus())
			{
				//Đm thằng nào lấy acc t @@
				return 2;
			}
			else
			{	//Ok acc này còn xài được
				listUsers[i].setStatus(true);	//T sẽ xí cái acc này
				return 1;
			}
		}
	}
	//Nếu tìm k ra thì thôi !!
	return 0;
}

User* ListUser::getUserByName(string name)
{
	User* res = NULL;
	for (int i = 0; listUsers.size(); i++)
	{
		if (name == listUsers[i].getUsername())
		{
			res = &listUsers[i];
		}
	}
	return res;
}