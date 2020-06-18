#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "pch.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
class User
{
private:
	string username;
	string password;
	bool status;		//Trạng thái hoạt động (1: online, 0: offline)
public:
	void setUsername(string username);
	void setPassword(string password);
	void setStatus(bool stt);
	string getUsername();
	string getPassword();
	bool getStatus();
	bool operator ==(User a);

	User();
};

class ListUser
{
private:
	vector<User> listUsers;
public:
	bool existUsername(string username);

	//Đọc file để lấy danh sách tk
	void updateList();
	//Xuất ra thông tin tk lên console(để test thôi)
	void writeAll();
	//Kiểm tra trạng thái để đăng nhập
	//Ret 1: có thể đăng nhập
	//Ret 0: không có user này(nhập sai)
	//Ret 2: user đang hoạt động
	int checkList(User guess);
	//Dùng username để get user
	User* getUserByName(string);
};